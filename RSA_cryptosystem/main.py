from keys import getKeys
from encrypt import encrypt
from decrypt import decrypt
from signature import sign, check_sign
from format import format64, gen64file
from binascii import b2a_base64

PRIME_LENGTH = 1024             # lenght in bits
INTEGER_LENGTH = 32             # lenght in bits
NUM_PRIMES = 100                # used to generate the first N primes in order to compare divisors in getKeys (faster to process 1024-bit primes)
DEBUG_KEY_GENERATION = False    # simple shows debug messages of a simple key generation

if __name__ == "__main__":
    zeros_not_padded = 0
    if not DEBUG_KEY_GENERATION:
        plain_text = input('\x1b[0;32;40m' + 'Insert your message to encrypt: ' + '\x1b[0m')
        if plain_text.count('0') > 0:
            zeros_not_padded += plain_text.count('0')
    else:
        plain_text = 'Somewhere over the rainbow'
    
    n, public_key, private_key = getKeys(PRIME_LENGTH, NUM_PRIMES, DEBUG_KEY_GENERATION)  # get public and secret keys pair

    if DEBUG_KEY_GENERATION:
        print('Public_key[e] =', public_key[0])
        print('Private_key[d] =', private_key[0])
        print('n =', n)

        e = public_key[0]
        x_block_size, y_block_size, p_block_size, hashed_ciphertext = encrypt(PRIME_LENGTH, plain_text, e, n, INTEGER_LENGTH)

        print('Hashed ciphertext =', hashed_ciphertext)

    else:
        e = public_key[0]
        x_block_size, y_block_size, p_block_size, hashed_ciphertext = encrypt(PRIME_LENGTH, plain_text, e, n, INTEGER_LENGTH)

        hashed_ciphertext_bytes = hashed_ciphertext.to_bytes(PRIME_LENGTH, byteorder='big', signed = False)
        hashed_ciphertext_bytes = b2a_base64(hashed_ciphertext_bytes, newline = False)

        cipher_text_size = len(hashed_ciphertext_bytes)
        for i in range(cipher_text_size):
            if hashed_ciphertext_bytes[i] != 65:
                save_i = i
                break

        print('\x1b[0;33;40m' + 'Hashed ciphertext' + '\x1b[0;34;40m' + ' <BASE64>' + '\x1b[0;32;40m' + ':' + '\x1b[0m', hashed_ciphertext_bytes[save_i:cipher_text_size])

        d = private_key[0]
        plain_text_after_decrypt = decrypt(PRIME_LENGTH, x_block_size, zeros_not_padded, p_block_size, hashed_ciphertext, d, n, INTEGER_LENGTH)
    
        print('\x1b[0;32;40m' + 'Plaintext after decrypting ciphertext:' + '\x1b[0m', str(plain_text_after_decrypt)[2:-1].strip('"\''))

        ### Signing messages ###
        encoded_plain_text = plain_text.encode()
        signature = sign(PRIME_LENGTH, encoded_plain_text, d, n)
        
        signature_bytes = b2a_base64(signature, newline = False)

        signature_size = len(signature_bytes)
        for i in range(signature_size):
            if signature_bytes[i] != 65:
                save_i = i
                break

        print('\x1b[0;33;40m' + 'Hashed signature' + '\x1b[0;34;40m' + ' <BASE64>' + '\x1b[0;32;40m' + ':' + '\x1b[0m', signature_bytes[save_i:signature_size])
        
        ### Checking if signature returns hashed plaintext ###
        check_signature = check_sign(INTEGER_LENGTH, encoded_plain_text, signature, e, n)
        print('\x1b[0;32;40m' + 'Verified signature =' + '\x1b[0m', check_signature)

        public_key_64, private_key_64, signature_64 = format64(e, d, signature, n)
        gen64file(public_key_64, private_key_64, signature_64)