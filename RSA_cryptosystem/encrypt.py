import string
from hashlib import sha3_256
from random import choices
from math import ceil

def sha3(byte_sequence: bytes, msg_bit_size, integer_len):
    bit_len = 8
    initial_hash_str = ''
    initial_hash_str = initial_hash_str.encode()
    initial_hash = sha3_256(initial_hash_str)
    hash_length = len(initial_hash.digest())
    hash_lim = ceil(msg_bit_size//hash_length)
    
    for c in range(0, hash_lim):
        k = c.to_bytes(integer_len//bit_len, byteorder='big', signed = False)
        k_hash = sha3_256(byte_sequence + k)
        initial_hash_str += k_hash.digest()
    final_hash = initial_hash_str[:msg_bit_size]
    return final_hash

def xorArray(var, key): # https://stackoverflow.com/questions/29408173/byte-operations-xor-in-python
    return bytes(a ^ b for a, b in zip(var, key))

# https://en.wikipedia.org/wiki/Optimal_asymmetric_encryption_padding
def oaep(prime_length: int, plain_text: string, integer_len: int):
    zeros = 0
    zero = '0'
    k0 = prime_length // integer_len
    k1 = prime_length // (2*integer_len)
    while True:
        zeros+= 1
        plain_text+= zero
        if zeros == k1:
            break
    
    r_list = []
    r = ''
    r_group = string.digits + string.ascii_lowercase
    r_list = choices(r_group, k = k0)
    for i in r_list:
        r += i
    
    r = r.encode()  # used in hash
    g = sha3(r, prime_length-k0, integer_len)
    plain_text = plain_text.encode()    # used to xor w/ r
    x = xorArray(g, plain_text)
    h = sha3_256(x)
    y = xorArray(r, h.digest())    

    # OAEP concat (x+y)
    size_x = len(x)
    size_y = len(y)
    size_p = len(x+y)
    return x + y, size_x, size_y, size_p


def encrypt(prime_length: int, plain_text: string, e, n, integer_length: int):
    padded_plaintext, x_block_size, y_block_size, p_block_size = oaep(prime_length, plain_text, integer_length)
    aux = int.from_bytes(padded_plaintext, byteorder='big', signed = False)
    padded_ciphertext = pow(aux, e, n)
    return x_block_size, y_block_size, p_block_size, padded_ciphertext