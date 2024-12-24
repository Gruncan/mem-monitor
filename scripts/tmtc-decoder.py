



class Malloc:
    def __init__(self, ptr, size):
        self.ptr = hex(ptr)
        self.size = size

    def __str__(self):
        return f"Malloc({self.size}) = {self.ptr}"


class Calloc:
    def __init__(self, nmemb, size, ptr):
        self.nmemb = nmemb
        self.size = size
        self.ptr = hex(ptr)

    def __str__(self):
        return f"Calloc({self.nmemb}, {self.size}) = {self.ptr}"

class ReAlloc:
    def __init__(self, ptr, size, new_ptr):
        self.ptr = hex(ptr)
        self.size = size
        self.new_ptr = hex(new_ptr)

    def __str__(self):
        return f"Realloc({self.ptr}, {self.size}) = {self.new_ptr}"

class ReAllocArray:
    def __init__(self, ptr, nmemb, size, new_ptr):
        self.ptr = hex(ptr)
        self.nmemb = nmemb
        self.size = size
        self.new_ptr = hex(new_ptr)

    def __str__(self):
        return f"Reallocarray({self.ptr}, {self.nmemb}, {self.size}) = {self.new_ptr}"

class Free:
    def __init__(self, ptr):
        self.ptr = hex(ptr)

    def __str__(self):
        return f"Free({self.ptr})"

class New:
    def __init__(self, ptr, size):
        self.ptr = hex(ptr)
        self.size = size

    def __str__(self):
        return f"New({self.size}) = {self.ptr}"


class NewNoThrow(New):

    def __str__(self):
        return f"New_nothrow({self.size}) = {self.ptr}"

class NewArray(New):

    def __str__(self):
        return f"New[]({self.size}) = {self.ptr}"


class NewArrayNoThrow(New):

    def __str__(self):
        return f"New[]_nothrow({self.size}) = {self.ptr}"

class Delete:

    def __init__(self, ptr):
        self.ptr = hex(ptr)

    def __str__(self):
        return f"Delete({self.ptr})"

class DeleteSized:

    def __init__(self, ptr, size):
        self.ptr = hex(ptr)
        self.size = size

    def __str__(self):
        return f"Delete_sized({self.ptr}, {self.size})"


class DeleteNoThrow(Delete):

    def __str__(self):
        return f"Delete_nothrow({self.ptr})"


class DeleteArray(Delete):

    def __str__(self):
        return f"Delete[]({self.ptr})"

class DeleteArraySized(DeleteSized):

    def __str__(self):
        return f"Delete[]_sized({self.ptr}, {self.size})"

class DeleteArrayNoThrow(DeleteArray):

    def __str__(self):
        return f"Delete[]_nothrow({self.ptr})"

class NewAligned(New):

    def __init__(self, ptr, size, align):
        super().__init__(ptr, size)
        self.align = align

    def __str__(self):
        return f"New_align({self.size}, {self.align}) = {self.ptr}"

class NewArrayAlign(NewAligned):

    def __str__(self):
        return f"New[]_align({self.size}, {self.align}) = {self.ptr}"


class DeleteAlign(Delete):

    def __init__(self, ptr, align):
        super().__init__(ptr)
        self.align = align

    def __str__(self):
        return f"Delete_align({self.ptr}, {self.align})"

class DeleteArrayAlign(DeleteAlign):

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

    def decode(self):
        if self.content is None:
            self.__load_file()

        file_length = len(self.content)
        index = 0

        data = []
        while index < file_length:
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

            inst = cls(*values)
            data.append(inst)
            print(inst)

        return data


decoder = TMtcDecoder("../memory-tracker/memory_tracker.tmtc")
d = decoder.decode()
print(d)
