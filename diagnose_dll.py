import platform
import struct

print("Python architecture (platform.machine()):", platform.machine())
print("Python pointer size (bits):", struct.calcsize("P") * 8)

with open("fastq_filter.dll", "rb") as f:
    data = f.read()

print("\nFile size:", len(data), "bytes")
print("First 2 bytes (should be b'MZ' for a real Windows binary):", data[0:2])

if data[0:2] == b"MZ":
    # The MZ header has a pointer to the PE header at offset 0x3C
    pe_offset = struct.unpack("<I", data[0x3C:0x40])[0]
    print("PE header offset:", pe_offset)
    pe_sig = data[pe_offset:pe_offset+4]
    print("PE signature (should be b'PE\\x00\\x00'):", pe_sig)

    if pe_sig == b"PE\x00\x00":
        machine = struct.unpack("<H", data[pe_offset+4:pe_offset+6])[0]
        machine_map = {
            0x8664: "AMD64 (64-bit x86, what we expect)",
            0x14c: "I386 (32-bit x86)",
            0xaa64: "ARM64",
            0x1c4: "ARMNT (32-bit ARM)",
        }
        print("Machine type field:", hex(machine), "->", machine_map.get(machine, "UNKNOWN"))
    else:
        print("!! This does not have a valid PE signature -- the file is not a real Windows binary/DLL.")
else:
    print("!! This file does not start with 'MZ' -- it is not a valid Windows executable/DLL at all.")
    print("First 50 bytes as text (in case it's actually an error message or HTML, etc.):")
    print(data[:50])
