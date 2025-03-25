import re
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datashader as ds
import holoviews as hv
from holoviews.operation.datashader import datashade
import colorcet as cc
from collections import defaultdict
from functools import partial
from tqdm import tqdm
import mmap
import io

# Configure Holoviews
hv.extension('bokeh')


class MemoryOperation:
    """Base class for memory operations"""

    def __init__(self, timestamp):
        self.timestamp = int(timestamp)

    def __getitem__(self, item):
        return getattr(self, item)

class AllocationOperation(MemoryOperation):
    """Base class for operations that allocate memory"""

    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp)
        self.ptr = ptr
        self.size = size
        self.type = 'alloc'


class DeallocationOperation(MemoryOperation):
    """Base class for operations that deallocate memory"""

    def __init__(self, timestamp, ptr):
        super().__init__(timestamp)
        self.ptr = ptr
        self.type = 'dealloc'


# Define specific operation classes
class Malloc(AllocationOperation):
    pass


class Calloc(AllocationOperation):
    def __init__(self, timestamp, nmemb, size, ptr):
        super().__init__(timestamp, ptr, nmemb * size)
        self.nmemb = nmemb


class ReAlloc(AllocationOperation):
    def __init__(self, timestamp, orig_ptr, size, ptr):
        super().__init__(timestamp, ptr, size)
        self.orig_ptr = orig_ptr
        self.is_resize = orig_ptr != 0


class ReAllocArray(AllocationOperation):
    def __init__(self, timestamp, orig_ptr, nmemb, size, ptr):
        super().__init__(timestamp, ptr, nmemb * size)
        self.orig_ptr = orig_ptr
        self.nmemb = nmemb
        self.is_resize = orig_ptr != 0


class Free(DeallocationOperation):
    pass


class New(AllocationOperation):
    pass


class NewNoThrow(AllocationOperation):
    pass


class NewArray(AllocationOperation):
    pass


class NewArrayNoThrow(AllocationOperation):
    pass


class Delete(DeallocationOperation):
    pass


class DeleteSized(DeallocationOperation):
    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp, ptr)
        self.size = size


class DeleteNoThrow(DeallocationOperation):
    pass


class DeleteArray(DeallocationOperation):
    pass


class DeleteArraySized(DeallocationOperation):
    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp, ptr)
        self.size = size


class DeleteArrayNoThrow(DeallocationOperation):
    pass


class NewAligned(AllocationOperation):
    def __init__(self, timestamp, ptr, size, align):
        super().__init__(timestamp, ptr, size)
        self.align = align


class NewArrayAlign(AllocationOperation):
    def __init__(self, timestamp, ptr, size, align):
        super().__init__(timestamp, ptr, size)
        self.align = align


class DeleteAlign(DeallocationOperation):
    def __init__(self, timestamp, ptr, align):
        super().__init__(timestamp, ptr)
        self.align = align


class DeleteArrayAlign(DeallocationOperation):
    def __init__(self, timestamp, ptr, align):
        super().__init__(timestamp, ptr)
        self.align = align


def parse_memory_logs_iterator(file_path, max_lines=None):
    """
    Stream parse memory logs without loading entire file into memory

    Parameters:
    file_path: Path to the log file
    max_lines: Maximum number of lines to process (None for all)

    Returns:
    Generator yielding memory operation objects
    """
    pattern = r'\[(\d+)\] ([\w\[\]\_]+)\((.*?)\)(?: = (0x[0-9a-f]+|\(nil\)))?'

    # Use mmap for efficient file reading
    with open(file_path, 'r') as f:
        # Memory map the file for efficient reading
        try:
            mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
            line_iterator = iter(mm.readline, b'')
            # Convert bytes to string
            line_iterator = (line.decode('utf-8') for line in line_iterator)
        except ValueError:
            # Fallback if mmap fails (e.g., empty file)
            f.seek(0)
            line_iterator = f

        count = 0
        for line in line_iterator:
            if max_lines and count >= max_lines:
                break

            match = re.match(pattern, line)
            if not match:
                continue

            timestamp, operation, params, result = match.groups()
            param_list = [p.strip() for p in params.split(',')]
            ptr = int(result, 16) if result and result != '(nil)' else 0

            obj = None

            if operation == 'malloc':
                size = int(param_list[0])
                obj = Malloc(timestamp, ptr, size)

            elif operation == 'calloc':
                nmemb = int(param_list[0])
                size = int(param_list[1])
                obj = Calloc(timestamp, nmemb, size, ptr)

            elif operation == 'realloc':
                orig_ptr = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                size = int(param_list[1])
                obj = ReAlloc(timestamp, orig_ptr, size, ptr)

            elif operation == 'reallocarray':
                orig_ptr = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                nmemb = int(param_list[1])
                size = int(param_list[2])
                obj = ReAllocArray(timestamp, orig_ptr, nmemb, size, ptr)

            elif operation == 'free':
                ptr_value = int(param_list[0], 16) if not param_list[0].__contains__('nil') else 0
                obj = Free(timestamp, ptr_value)

            elif operation == 'new':
                size = int(param_list[0])
                obj = New(timestamp, ptr, size)

            elif operation == 'new_nothrow':
                size = int(param_list[0])
                obj = NewNoThrow(timestamp, ptr, size)

            elif operation == 'new[]':
                size = int(param_list[0])
                obj = NewArray(timestamp, ptr, size)

            elif operation == 'new[]_nothrow':
                size = int(param_list[0])
                obj = NewArrayNoThrow(timestamp, ptr, size)

            elif operation == 'delete':
                obj = Delete(timestamp, ptr)

            elif operation == 'delete_sized':
                size = int(param_list[1])
                ptr_value = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                obj = DeleteSized(timestamp, ptr_value, size)

            elif operation == 'delete_nothrow':
                obj = DeleteNoThrow(timestamp, ptr)

            elif operation == 'delete[]':
                obj = DeleteArray(timestamp, ptr)

            elif operation == 'delete[]_sized':
                size = int(param_list[1])
                ptr_value = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                obj = DeleteArraySized(timestamp, ptr_value, size)

            elif operation == 'delete[]_nothrow':
                obj = DeleteArrayNoThrow(timestamp, ptr)

            elif operation == 'new_align':
                size = int(param_list[0])
                align = int(param_list[1])
                obj = NewAligned(timestamp, ptr, size, align)

            elif operation == 'new[]_align':
                size = int(param_list[0])
                align = int(param_list[1])
                obj = NewArrayAlign(timestamp, ptr, size, align)

            elif operation == 'delete_align':
                align = int(param_list[1])
                ptr_value = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                obj = DeleteAlign(timestamp, ptr_value, align)

            elif operation == 'delete[]_align':
                align = int(param_list[1])
                ptr_value = int(param_list[0], 16) if param_list[0] != '(nil)' else 0
                obj = DeleteArrayAlign(timestamp, ptr_value, align)

            if obj:
                count += 1
                yield obj


def chunk_processor(operations, chunk_size=1000000):
    """Process operations in chunks to avoid memory issues"""
    chunk = []
    for op in operations:
        chunk.append(op)
        if len(chunk) >= chunk_size:
            yield chunk
            chunk = []
    if chunk:
        yield chunk


def process_memory_operations(file_path, output_prefix='memory_analysis', chunk_size=1000000, max_lines=None):
    """
    Process memory operations efficiently using generator and chunking

    Parameters:
    file_path: Path to the log file
    output_prefix: Prefix for output files
    chunk_size: Size of processing chunks
    max_lines: Maximum number of lines to process (None for all)

    Returns:
    Dictionary with summary statistics
    """
    print(f"Processing file: {file_path}")

    # Initialize tracking data structures
    address_to_size = {}
    active_allocations = defaultdict(int)
    time_to_memory = defaultdict(int)
    time_to_allocs = defaultdict(int)
    all_addresses = set()
    all_timestamps = set()

    # Track min/max values
    min_address = float('inf')
    max_address = 0
    min_timestamp = float('inf')
    max_timestamp = 0

    # First pass: scan the file to get global statistics
    print("First pass: scanning file for global statistics...")

    # Create a generator for memory operations
    operations = parse_memory_logs_iterator(file_path, max_lines)

    # Process operations in chunks
    total_ops = 0
    for chunk in chunk_processor(operations, chunk_size):
        total_ops += len(chunk)

        # Process each operation in the chunk
        for op in chunk:
            # Update global statistics
            if hasattr(op, 'ptr') and op.ptr > 0:
                min_address = min(min_address, op.ptr)
                max_address = max(max_address, op.ptr)
                all_addresses.add(op.ptr)

            if hasattr(op, 'timestamp'):
                min_timestamp = min(min_timestamp, op.timestamp)
                max_timestamp = max(max_timestamp, op.timestamp)
                all_timestamps.add(op.timestamp)

        # Print progress
        print(f"Processed {total_ops} operations")

    print(f"Total operations: {total_ops}")
    print(f"Address range: 0x{min_address:x} - 0x{max_address:x}")
    print(f"Timestamp range: {min_timestamp} - {max_timestamp}")

    # Calculate time bins for summarization
    num_bins = min(1000, len(all_timestamps))
    time_range = max_timestamp - min_timestamp
    bin_size = time_range / num_bins if time_range > 0 else 1

    # Reset and start second pass for actual processing
    operations = parse_memory_logs_iterator(file_path, max_lines)

    # Process allocations to generate memory usage data
    print("Second pass: processing allocations and generating statistics...")

    # Create data structures for visualization
    alloc_data = []
    memory_over_time = []

    # Track the state of allocations
    active_allocs = {}

    # Process operations in chunks
    for chunk in tqdm(chunk_processor(operations, chunk_size), desc="Processing chunks"):
        for op in chunk:
            timestamp = op.timestamp

            if hasattr(op, 'type') and op.type == 'alloc' and op.ptr > 0:
                if hasattr(op, 'is_resize') and op.is_resize:
                    if op.orig_ptr in active_allocs:
                        del active_allocs[op.orig_ptr]

                if op.ptr in active_allocs:
                    continue

                active_allocs[op.ptr] = {
                    'timestamp': timestamp,
                    'size': op.size,
                    'address': op.ptr
                }

                alloc_data.append({
                    'timestamp': timestamp,
                    'address': op.ptr,
                    'size': op.size,
                    'type': 'alloc'
                })

            elif hasattr(op, 'type') and op.type == 'dealloc' and op.ptr > 0:
                if op.ptr not in active_allocs:
                    continue

                alloc_info = active_allocs[op.ptr]

                alloc_data.append({
                    'timestamp': timestamp,
                    'address': op.ptr,
                    'size': alloc_info['size'],
                    'type': 'dealloc'
                })

                del active_allocs[op.ptr]

            # Calculate memory usage for this timestamp
            current_memory = sum(a['size'] for a in active_allocs.values())
            memory_over_time.append({
                'timestamp': timestamp,
                'memory_used': current_memory,
                'allocation_count': len(active_allocs)
            })

    # Convert to DataFrames for more efficient processing
    print("Converting data to DataFrames...")
    alloc_df = pd.DataFrame(alloc_data)
    memory_df = pd.DataFrame(memory_over_time)

    # Sample data if necessary
    max_points = 100000
    if len(alloc_df) > max_points:
        print(f"Sampling allocation data from {len(alloc_df)} to {max_points} points")
        alloc_df = alloc_df.sample(max_points)

    if len(memory_df) > max_points:
        print(f"Sampling memory usage data from {len(memory_df)} to {max_points} points")
        memory_df = memory_df.sample(max_points)

    # Sort by timestamp
    alloc_df = alloc_df.sort_values('timestamp')
    memory_df = memory_df.sort_values('timestamp')

    # Summarize memory usage over time
    print("Creating memory usage summary...")
    memory_summary = summarize_memory_usage(memory_df, num_bins)
    memory_summary.to_csv(f"{output_prefix}_summary.csv", index=False)

    # Create visualizations
    print("Creating visualizations...")
    create_memory_visualizations(alloc_df, memory_summary, output_prefix)

    # Create interactive datashader visualization
    print("Creating interactive visualization...")
    create_interactive_visualization(alloc_df, output_prefix)

    return {
        'total_operations': total_ops,
        'address_range': (min_address, max_address),
        'timestamp_range': (min_timestamp, max_timestamp),
        'summary_file': f"{output_prefix}_summary.csv",
        'summary_plot': f"{output_prefix}_summary.png",
        'timeline_html': f"{output_prefix}_timeline.html"
    }


def summarize_memory_usage(memory_df, num_bins):
    """Create a summary of memory usage over time"""
    # Use pandas resampling to create time bins
    if not memory_df.empty:
        # Find time range
        min_time = memory_df['timestamp'].min()
        max_time = memory_df['timestamp'].max()
        time_range = max_time - min_time

        # Create time bins
        bins = np.linspace(min_time, max_time, num_bins + 1)

        # Assign each timestamp to a bin
        memory_df['time_bin'] = pd.cut(memory_df['timestamp'], bins=bins, labels=False)

        # Group by time bin and calculate statistics
        summary = memory_df.groupby('time_bin').agg({
            'timestamp': 'mean',
            'memory_used': ['mean', 'max', 'min'],
            'allocation_count': ['mean', 'max', 'min']
        }).reset_index()

        # Flatten the multi-index columns
        summary.columns = ['time_bin', 'timestamp', 'memory_used_mean', 'memory_used_max', 'memory_used_min',
                           'allocation_count_mean', 'allocation_count_max', 'allocation_count_min']

        return summary
    else:
        # Return empty DataFrame with expected columns
        return pd.DataFrame(columns=['time_bin', 'timestamp', 'memory_used_mean', 'memory_used_max', 'memory_used_min',
                                     'allocation_count_mean', 'allocation_count_max', 'allocation_count_min'])


def create_memory_visualizations(alloc_df, memory_summary, output_prefix):
    """Create visualizations for memory usage"""
    # Create figure with subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 12))

    # Plot memory usage over time
    ax1.plot(memory_summary['timestamp'], memory_summary['memory_used_mean'], '-', linewidth=2, label='Mean')
    ax1.fill_between(memory_summary['timestamp'],
                     memory_summary['memory_used_min'],
                     memory_summary['memory_used_max'],
                     alpha=0.3, label='Min/Max Range')
    ax1.set_title('Memory Usage Over Time')
    ax1.set_xlabel('Timestamp')
    ax1.set_ylabel('Memory Used (bytes)')
    ax1.grid(True)
    ax1.legend()

    # Plot allocation count over time
    ax2.plot(memory_summary['timestamp'], memory_summary['allocation_count_mean'], '-', color='green', linewidth=2,
             label='Mean')
    ax2.fill_between(memory_summary['timestamp'],
                     memory_summary['allocation_count_min'],
                     memory_summary['allocation_count_max'],
                     color='green', alpha=0.3, label='Min/Max Range')
    ax2.set_title('Number of Active Allocations Over Time')
    ax2.set_xlabel('Timestamp')
    ax2.set_ylabel('Count')
    ax2.grid(True)
    ax2.legend()

    plt.tight_layout()
    plt.savefig(f"{output_prefix}_summary.png", dpi=300)
    plt.close()

    # Create heatmap of allocations
    if not alloc_df.empty and 'address' in alloc_df.columns and 'timestamp' in alloc_df.columns:
        # Create a heatmap of allocations by address and time
        plt.figure(figsize=(12, 8))

        # Filter to just allocation events
        allocs_only = alloc_df[alloc_df['type'] == 'alloc']

        # Create a 2D histogram
        if not allocs_only.empty:
            plt.hist2d(allocs_only['timestamp'], allocs_only['address'], bins=50, cmap='viridis')
            plt.colorbar(label='Number of Allocations')
            plt.title('Allocation Heatmap')
            plt.xlabel('Timestamp')
            plt.ylabel('Memory Address')
            plt.savefig(f"{output_prefix}_heatmap.png", dpi=300)
            plt.close()


def create_interactive_visualization(alloc_df, output_prefix):
    """Create an interactive visualization using Holoviews and Datashader"""
    if not alloc_df.empty and 'address' in alloc_df.columns and 'timestamp' in alloc_df.columns:
        # Create a dataset with allocations
        allocs = alloc_df[alloc_df['type'] == 'alloc'].copy()
        deallocs = alloc_df[alloc_df['type'] == 'dealloc'].copy()

        if not allocs.empty and not deallocs.empty:
            # Create a points dataset
            points = hv.Points(allocs, kdims=['timestamp', 'address'], vdims=['size'])

            # Apply datashader for efficient rendering
            shaded = datashade(points, cmap=cc.fire, aggregator=ds.count()).opts(
                width=1200,  # Increase width
                height=800   # Increase height
            )

            # Save as HTML
            hv.save(shaded, f"{output_prefix}_timeline.html")


def count_lines(file_path):
    """Count the number of lines in a file efficiently"""
    with open(file_path, 'rb') as f:
        # Memory map the file for efficient reading
        try:
            mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
            return mm.count(b'\n')
        except ValueError:
            # Fallback if mmap fails (e.g., empty file)
            f.seek(0)
            return sum(1 for _ in f)


def analyze_memory_log(file_path, output_prefix='memory_analysis', max_lines=None):
    """Main function to analyze memory log"""
    # Count total lines in file
    # total_lines = count_lines(file_path)
    # print(f"Total lines in file: {total_lines}")

    # Process the file
    results = process_memory_operations(file_path, output_prefix, max_lines=max_lines)

    print(f"Analysis complete. Results:")
    for key, value in results.items():
        print(f"  {key}: {value}")

    return results


# Example usage
if __name__ == "__main__":
    # File path to your memory log file
    file_path = "/home/duncan/Development/C/mem-monitor/components/common/mtc/uwb_complete_chrome_raw.txt"

    # Analyze the first 1 million lines (set to None for all lines)
    analyze_memory_log(file_path, max_lines=2000000)