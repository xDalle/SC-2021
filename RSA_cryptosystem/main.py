from keys import getKeys
from encrypt import encrypt
from decrypt import decrypt
from signature import sign, check_sign
from format import format64, gen64file

PRIME_LENGTH = 1024             # lenght in bits
INTEGER_LENGTH = 32             # lenght in bits
NUM_PRIMES = 100                # used to generate the first N primes in order to compare divisors in getKeys (faster to process 1024-bit primes)
DEBUG_KEY_GENERATION = False    # simple shows debug messages of a simple key generation

if __name__ == "__main__":
    zeros_not_padded = 0
    if not DEBUG_KEY_GENERATION:
        plain_text = input('Insert your message to encrypt: ')
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

        print('Hashed ciphertext <int> =', hashed_ciphertext)

        d = private_key[0]
        plain_text_after_decrypt = decrypt(PRIME_LENGTH, x_block_size, zeros_not_padded, p_block_size, hashed_ciphertext, d, n, INTEGER_LENGTH)
    
        print('Plaintext after decrypt ciphertext =', str(plain_text_after_decrypt)[2:-1].strip('"\''))

        ### Signing messages ###
        encoded_plain_text = plain_text.encode()
        signature = sign(PRIME_LENGTH, encoded_plain_text, d, n)

        ### Checking if signature returns hashed plaintext ###
        check_signature = check_sign(INTEGER_LENGTH, encoded_plain_text, signature, e, n)
        print('Verified signature =', check_signature)

        public_key_64, private_key_64, signature_64 = format64(e, d, signature)
        gen64file(public_key_64, private_key_64, signature_64)