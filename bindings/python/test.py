import ctypes

lib = ctypes.CDLL('./test.dll')

result = lib.add(1, 2)
print(result)