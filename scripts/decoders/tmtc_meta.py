import re
from datetime import datetime, timedelta

from tmtc_decoder_wrapper import TMtcPoint



def format_timestamp(func):
    def _wrapper(cls, *args, **kwargs):
        if not isinstance(cls, AllocationPoint):
            raise Exception("Can't format timestamp for unknown class")
        return cls._format_timestamp() + func(cls, *args, **kwargs)
    return _wrapper

class AllocationPoint:

    def __init__(self, timestamp):
        self.timestamp = int(timestamp)
        self.timestamp_end = None
        self.allocation_index_start = 0
        self.allocation_index_end = 0

    def __repr__(self):
        return f"{self.__class__.__name__}: {getattr(self, 'ptr')}"

    def _format_timestamp(self):
        return f"[{self.timestamp.strftime('%Y-%m-%d %H:%M:%S.%f')}] "

    def __json__(self):
        values = dict(self.__dict__)
        del values["allocation_index_start"]
        del values["allocation_index_end"]
        return values

class Malloc(AllocationPoint):
    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp)
        self.ptr = hex(ptr)
        self.size = size

    @format_timestamp
    def __str__(self):
        return f"Malloc({self.size}) = {self.ptr}"


class Calloc(AllocationPoint):
    def __init__(self, timestamp, nmemb, size, ptr):
        super().__init__(timestamp)
        self.nmemb = nmemb
        self.size = size
        self.ptr = hex(ptr)

    @format_timestamp
    def __str__(self):
        return f"Calloc({self.nmemb}, {self.size}) = {self.ptr}"

class ReAlloc(AllocationPoint):
    def __init__(self, timestamp, ptr, size, new_ptr):
        super().__init__(timestamp)
        self.ptr = hex(ptr)
        self.size = size
        self.new_ptr = hex(new_ptr)

    @format_timestamp
    def __str__(self):
        return f"Realloc({self.ptr}, {self.size}) = {self.new_ptr}"

class ReAllocArray(AllocationPoint):
    def __init__(self, timestamp, ptr, nmemb, size, new_ptr):
        super().__init__(timestamp)
        self.ptr = hex(ptr)
        self.nmemb = nmemb
        self.size = size
        self.new_ptr = hex(new_ptr)

    @format_timestamp
    def __str__(self):
        return f"Reallocarray({self.ptr}, {self.nmemb}, {self.size}) = {self.new_ptr}"

class Free(AllocationPoint):
    def __init__(self, timestamp, ptr):
        super().__init__(timestamp)
        self.ptr = hex(ptr)

    @format_timestamp
    def __str__(self):
        return f"Free({self.ptr})"

class New(AllocationPoint):
    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp)
        self.ptr = hex(ptr)
        self.size = size

    @format_timestamp
    def __str__(self):
        return f"New({self.size}) = {self.ptr}"


class NewNoThrow(New):

    @format_timestamp
    def __str__(self):
        return f"New_nothrow({self.size}) = {self.ptr}"

class NewArray(New):

    @format_timestamp
    def __str__(self):
        return f"New[]({self.size}) = {self.ptr}"


class NewArrayNoThrow(New):

    @format_timestamp
    def __str__(self):
        return f"New[]_nothrow({self.size}) = {self.ptr}"

class Delete(AllocationPoint):

    def __init__(self, timestamp, ptr):
        super().__init__(timestamp)
        self.ptr = hex(ptr)

    @format_timestamp
    def __str__(self):
        return f"Delete({self.ptr})"

class DeleteSized(AllocationPoint):

    def __init__(self, timestamp, ptr, size):
        super().__init__(timestamp)
        self.ptr = hex(ptr)
        self.size = size

    @format_timestamp
    def __str__(self):
        return f"Delete_sized({self.ptr}, {self.size})"


class DeleteNoThrow(Delete):

    @format_timestamp
    def __str__(self):
        return f"Delete_nothrow({self.ptr})"


class DeleteArray(Delete):

    @format_timestamp
    def __str__(self):
        return f"Delete[]({self.ptr})"

class DeleteArraySized(DeleteSized):

    @format_timestamp
    def __str__(self):
        return f"Delete[]_sized({self.ptr}, {self.size})"

class DeleteArrayNoThrow(DeleteArray):

    @format_timestamp
    def __str__(self):
        return f"Delete[]_nothrow({self.ptr})"

class NewAligned(New):

    def __init__(self, timestamp, ptr, size, align):
        super().__init__(timestamp, ptr, size)
        self.align = align

    @format_timestamp
    def __str__(self):
        return f"New_align({self.size}, {self.align}) = {self.ptr}"

class NewArrayAlign(NewAligned):

    @format_timestamp
    def __str__(self):
        return f"New[]_align({self.size}, {self.align}) = {self.ptr}"


class DeleteAlign(Delete):

    def __init__(self, timestamp, ptr, align):
        super().__init__(timestamp, ptr)
        self.align = align

    @format_timestamp
    def __str__(self):
        return f"Delete_align({self.ptr}, {self.align})"

class DeleteArrayAlign(DeleteAlign):

    @format_timestamp
    def __str__(self):
        return f"Delete[]_align({self.ptr}, {self.align})"


_class_map = {0x0: Malloc, 0x1: Calloc, 0x2: ReAlloc, 0x3: ReAllocArray, 0x4: Free, 0x5: New, 0x6: NewNoThrow,
              0x7: NewArray, 0x8: NewArrayNoThrow, 0x9: Delete, 0xa: DeleteSized, 0xb: DeleteNoThrow,
              0xc: DeleteArray, 0xd: DeleteArraySized, 0xe: DeleteArrayNoThrow, 0xf: NewAligned,
              0x10: NewArrayAlign, 0x11: DeleteAlign, 0x12: DeleteArrayAlign}

def load_from_point(point: TMtcPoint, time: int):
    clz = _class_map.get(point.key, None)
    if clz is None:
        return None, 0
    try:
        return clz(point.time_offset + time, *point.values), point.time_offset
    except Exception as e:
        print(str(e))


def get_peak_memory_raw_log(log_data):
    pattern = r'\[(\d+)\] ([\w\[\]\_]+)\((.*?)\)(?: = (0x[0-9a-f]+|\(nil\)))?'

    size = 0
    size_max = 0
    address_map = {}

    for line in log_data.strip().split('\n'):
        match = re.match(pattern, line)
        if not match:
            print(line)
            continue

        timestamp, operation, params, ptr = match.groups()
        param_list = [p.strip() for p in params.split(',')]

        if operation in ('malloc', 'new', 'new_nothrow', 'new[]', 'new[]_nothrow', 'new_align', 'new[]_align'):
            if ptr in address_map:
                continue
            alloc_size = int(param_list[0])
            address_map[ptr] = alloc_size
            size += alloc_size
            if size > size_max:
                print(address_map)
                size_max = size
        elif operation in ('free', 'delete', 'delete_sized', 'delete_nothrow', 'delete[]', 'delete[]_sized', 'delete[]_nothrow', 'delete_align', 'delete[]_align'):
            ptr = params[0]
            if ptr not in address_map:
                continue
            alloc_size = address_map[ptr]
            del address_map[ptr]
            size -= alloc_size
        elif operation == 'calloc':
            if ptr not in address_map:
                continue
            nmemb = int(param_list[0])
            alloc_size = int(param_list[1])
            size += alloc_size
            if size > size_max:
                size_max = size
        elif operation in ('realloc', 'reallocarray'):
            print("Realloc is used!")
        else:
            print("Unknown opeation!", operation)

    return size_max

def parse_memory_logs(log_data):
    pattern = r'\[(\d+)\] ([\w\[\]\_]+)\((.*?)\)(?: = (0x[0-9a-f]+|\(nil\)))?'

    count = 0

    for line in log_data.strip().split('\n'):
        if count == -1:
            break
        match = re.match(pattern, line)
        if not match:
            print(line)
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

