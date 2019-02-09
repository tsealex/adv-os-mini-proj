import random

MAX_INT = 2 << 63 - 1


def create_records(num_records):
    with open('records', 'wb') as f:
        for i in range(num_records):
            f.write(i.to_bytes(8, byteorder='little'))
            f.write(random.randint(0, MAX_INT).to_bytes(8, byteorder='little'))


# For verifying the script is working.
# with open('records', 'rb') as f:
# 	b = f.read()
# 	for i in range(0, len(b), 16):
# 		k = int.from_bytes(b[i:i+8], byteorder='little')
# 		v = int.from_bytes(b[i+8:i+16], byteorder='little')
# 		print(k, v)


# Create a list of records, each consists two 8-byte integers.
create_records(2 ** 30 // 16)  # ~2 GB data = ~67M records
