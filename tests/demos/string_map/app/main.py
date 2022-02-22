# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT

# Note: this demo doesn't work with ASan enabled on GCC 9.3.0 because of
# https://bugs.llvm.org/show_bug.cgi?id=39641

import ctypes
import os
from ctypes import CFUNCTYPE, c_char_p, c_int, c_void_p, c_char

lib_path = os.getenv("FELTPLUGINSYSTEM_HOST_PATH")
assert lib_path is not None, "FELTPLUGINSYSTEM_HOST_PATH must be set"
lib_path = os.path.join(lib_path, "libfeltpluginsystem-demo-string_map-host.so")

host = None


class fpdemo_String_s(ctypes.Structure):
    _fields_ = [
        ("create", CFUNCTYPE(c_int, c_char_p, c_void_p)),
        ("release", CFUNCTYPE(None, c_void_p)),
        ("assign_cstr", CFUNCTYPE(c_int, c_char_p, c_void_p, c_char_p)),
        ("assign_StringView", CFUNCTYPE(c_int, c_char_p, c_void_p, c_void_p)),
        ("c_str", CFUNCTYPE(c_char_p, c_void_p)),
        ("at", CFUNCTYPE(c_int, c_char_p, c_char_p, c_void_p, c_int)),
    ]


class CString:
    def __init__(self, s: str):
        self.__csuite = host.fpdemo_String_suite()
        self.__cerr = ctypes.create_string_buffer(500)
        self.__chandle = c_void_p()

        code = self.__csuite.create(self.__cerr, ctypes.byref(self.__chandle))
        if code != 0:
            raise RuntimeError("Error code '%s' from C with message: '%s'" % (code, self.__cerr))

        self.__csuite.assign_cstr(self.__cerr, self.__chandle, s.encode())

    def __del__(self):
        self.__csuite.release(self.__chandle)

    def c_str(self):
        return self.__csuite.c_str(self.__chandle)

    def at(self, pos: int):
        out = c_char()
        code = self.__csuite.at(self.__cerr, ctypes.byref(out), self.__chandle, pos)
        if code != 0:
            raise RuntimeError(
                "Error code '%s' from C with message: '%s'" % (code, self.__cerr.value))
        return out.value


if __name__ == "__main__":
    host = ctypes.CDLL(lib_path)
    host.fpdemo_String_suite.restype = fpdemo_String_s

    cstr = CString("some data")

    print(cstr.c_str())

    try:
        for n in range(100):
            print(cstr.at(n))
    except RuntimeError as ex:
        print(ex)
