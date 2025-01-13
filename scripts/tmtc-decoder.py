
from datetime import datetime, timedelta
from pprint import pprint


def format_timestamp(func):
    def _wrapper(cls, *args, **kwargs):
        if not isinstance(cls, AllocationPoint):
            raise Exception("Can't format timestamp for unknown class")
        return cls._format_timestamp() + func(cls, *args, **kwargs)
    return _wrapper

class AllocationPoint:

    def __init__(self, timestamp):
        self.timestamp = timestamp

    def __repr__(self):
        return f"{self.__class__.__name__}: {getattr(self, 'ptr')}"

    def _format_timestamp(self):
        return f"[{self.timestamp.strftime('%Y-%m-%d %H:%M:%S.%f')}] "

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

class TMtcDecoder:

    _class_map = {0x0: Malloc, 0x1: Calloc, 0x2: ReAlloc, 0x3: ReAllocArray, 0x4: Free, 0x5: New, 0x6: NewNoThrow,
                  0x7: NewArray, 0x8: NewArrayNoThrow, 0x9: Delete, 0xa: DeleteSized, 0xb: DeleteNoThrow,
                  0xc: DeleteArray, 0xd: DeleteArraySized, 0xe: DeleteArrayNoThrow, 0xf: NewAligned,
                  0x10: NewArrayAlign, 0x11: DeleteAlign, 0x12: DeleteArrayAlign}

    def __init__(self, filename):
        self.content = None
        self.filename = filename

    def __load_file(self):
        with open(self.filename, "rb") as f:
            self.content = f.read()

    def __decode_time(self, time):
        tv_sec = ((time[0] << 24)
                    | (time[1] << 16)
                    | (time[2] << 8)
                    | (time[3]))
        tv_usec = ((time[4] << 24)
                    | (time[5] << 16)
                    | (time[6] << 8)
                    | time[7])
        dt = datetime.fromtimestamp(tv_sec)
        return dt + timedelta(microseconds=tv_usec)


    def parse(self):
        if self.content is None:
            self.__load_file()

        file_length = len(self.content)
        index = 0

        while index < file_length:
            time = self.__decode_time(self.content[index:index+8])
            index += 8
            key = self.content[index]
            length = self.content[index+1]
            cls = TMtcDecoder._class_map.get(key, None)
            if cls is None:
                raise Exception("Failed to decode unknown key")
            index += 2
            values = []
            for i in range(0, length * 8, 8):
                value = 0
                for j in range(8):
                    value |= self.content[index + i + j] << (8 * (7 - j))
                values.append(value)
            index += length * 8

            inst = cls(time, *values)
            pprint(inst)
            yield inst

    def decode(self):
        allocations = self.parse()
        addresses = {}
        while (curr := next(allocations, None)) is not None:
            if curr.ptr not in addresses:
                addresses[curr.ptr] = [curr]
            else:
                addresses[curr.ptr].append(curr)

        pprint(addresses)

decoder = TMtcDecoder("C:\\Users\\DJ223\\Downloads\\memory_tracker.tmtc")
decoder.decode()

