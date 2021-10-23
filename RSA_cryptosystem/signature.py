from hashlib import sha3_256

# https://en.wikipedia.org/w/index.php?title=RSA_(cryptosystem)#Signing_messages
def sign(prime_length: int, plain_text: bytes, d, n):
    h = sha3_256(plain_text)
    aux = int.from_bytes(h.digest(), byteorder='big', signed = False)
    k = pow(aux, d, n)
    signature = k.to_bytes(prime_length, byteorder='big', signed = False)
    return signature

def check_sign(integer_length: int, plain_text: bytes, signature: bytes, e, n):
    aux = int.from_bytes(signature, byteorder='big', signed = False)
    k = pow(aux, e, n)
    check = k.to_bytes(integer_length, byteorder='big', signed = False)
    h = sha3_256(plain_text)
    signature = h.digest()

    return check == signature