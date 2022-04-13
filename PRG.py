from Crypto.Cipher import AES
from Crypto.Util import Counter
from bitstring import BitArray
import random

def PRG_int(seed):
    # the input is an integer corresponding to a binary string of 127 bits
    # the output is an integer corresponding to a random binary string exactly double the size of the input plus two bits
    # we want to concatenate the outputs of the AES encryptions of 0...0 and 1...1 using the input as key
    key = seed<<1
    aes = AES.new((key).to_bytes(16,byteorder='big'), AES.MODE_CTR, counter=Counter.new(128))
    left = aes.encrypt(bytes([0])*16)
    right = aes.encrypt(bytes([255])*16)
    sample = left + right
    return int.from_bytes(sample,byteorder='big')

def PRG_bytes(seed):
    # the input is 16 bytes (but not \xff*16)
    # the output is 32 bytes
    # we want to concatenate the outputs of the AES encryptions of 0...0 and 1...1 using the input as key
    key = int.from_bytes(seed,byteorder='big')<<1
    aes = AES.new((key).to_bytes(16,byteorder='big'), AES.MODE_CTR, counter=Counter.new(128))
    left = aes.encrypt(bytes([0])*16)
    right = aes.encrypt(bytes([255])*16)
    sample = left + right
    return sample

def PRG_str(seed):
    # the input is an integer corresponding to a binary string of 127 bits
    # the output is an integer corresponding to a random binary string exactly double the size of the input plus two bits
    # we want to concatenate the outputs of the AES encryptions of 0...0 and 1...1 using the input as key
    key = int(seed+'0',2)
    aes = AES.new((key).to_bytes(16,byteorder='big'), AES.MODE_CTR, counter=Counter.new(128))
    left = aes.encrypt(bytes([0])*16)
    right = aes.encrypt(bytes([255])*16)
    sample = left + right
    return BitArray(sample).bin

def PRG(seed):
    # If you want the output to be the same type as the input
    sample=None
    if type(seed) == int:
        sample = PRG_int(seed)
    elif type(seed) == bytes:
        sample = PRG_bytes(seed)
    elif type(seed) == str:
        sample = PRG_str(seed)
    return sample

def PRG_bytes(seed):
    # If you want the output to always be in bytes since it is the type of the AES output (the user can change it to whatever type he wishes afterwards)
    if type(seed) == int:
        key = seed<<1
    elif type(seed) == bytes:
        key = int.from_bytes(seed,byteorder='big')<<1
    elif type(seed) == str:
        key = int(seed+'0',2)
    else:
        print("Wrong type for the input of PRG function")
        return None
    aes = AES.new((key).to_bytes(16,byteorder='big'), AES.MODE_CTR, counter=Counter.new(128))
    left = aes.encrypt(bytes([0])*16)
    right = aes.encrypt(bytes([255])*16)
    return left + right

# ___ALTERNATIVES___
#
# IS BYTES_FROMHEX(HEX(INT(SEED,2))) FASTER THAN (INT(SEED,2)).TO_BYTES(16,BYTEORDER='BIG') ?
# def PRG_str(seed):
#     key = hex(int(seed+'0',2))[2:]
#     aes = AES.new(bytes.fromhex(key), AES.MODE_CTR, counter=Counter.new(128))
#     left = BitArray(bytes=aes.encrypt(bytes.fromhex('0'*32)))
#     right = BitArray(bytes=aes.encrypt(bytes.fromhex('f'*32)))
#     sample = left.bin + right.bin
#     return sample
#
# WHAT IF WE CONCATENATE THE OUTPUTS OF THE AES ENCRYPTIONS OF THE INPUT USING FIXED KEYS ?
# link : https://www.usenix.org/system/files/conference/nsdi17/nsdi17-wang-frank.pdf
# def PRG_int(seed, k0, k1):
#     aes_0 = AES.new((k0).to_bytes(16,byteorder='big'))
#     aes_1 = AES.new((k1).to_bytes(16,byteorder='big'))
#     argument = (seed).to_bytes(16,byteorder='big')
#     left = BitArray(bytes=aes_0.encrypt(argument)) 
#     right = BitArray(bytes=aes_1.encrypt(argument))
#     int_left = left.int ^ int_seed
#     int_right = right.int ^ int_seed

if __name__ == "__main__":
    # seed = random.getrandbits(127)
    # print(seed)

    int_seed = 74597119449584372028477538070780248601
    bytes_seed = (int_seed).to_bytes(16,byteorder='big')
    str_seed = bin(int_seed)[2:]

    int_sample = PRG(int_seed)
    bytes_sample = PRG(bytes_seed)
    str_sample = PRG(str_seed)

    print(int_sample)
    print(str_sample)
    print(bytes_sample)

    print(int.from_bytes(bytes_sample, byteorder='big') == int_sample)
    print(int(str_sample,2) == int_sample)
    print(BitArray(bytes_sample).bin == str_sample)