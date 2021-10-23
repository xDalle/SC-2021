from encrypt import xorArray, sha3
from hashlib import sha3_256

def rreplace(s, old, new, occurrence):
    li = s.rsplit(old, occurrence)
    return new.join(li)

# Reverse encrypt oaep
def oaep(prime_length: int, x_block: bytes, y_block:bytes, zeros, integer_length):
    h = sha3_256(x_block)
    r = xorArray(y_block, h.digest())
    g = sha3(r, prime_length-(prime_length//integer_length), integer_length)
    plain_text = xorArray(x_block, g)
    plain_text = str(plain_text)

    plain_text = rreplace(plain_text, '0', '', plain_text.count('0') - zeros)   # remove padding zeros
    return plain_text

def decrypt(prime_length: int, size_x, zeros, size_p, cipher_text, d, n, integer_length: int):
    padded_plaintext = pow(cipher_text, d, n)
    aux = padded_plaintext.to_bytes(size_p, byteorder='big', signed = False)
    first_block = aux[0:size_x]
    second_block = aux[size_x:size_p]   # remember that size_p = size_x + size_y
    original_plaintext = oaep(prime_length, first_block, second_block, zeros, integer_length)
    return original_plaintext