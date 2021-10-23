from binascii import b2a_base64

def gen64file(public_key, private_key, signature):
    print('Generating file `out.md` containing public key, private key and signature in base64.')
    f = open('out.md', 'w')
    f.write('===== BEGIN RSA PUBLIC KEY =====\n')
    f.write(str(public_key)[2:-1].strip('"\''))
    f.write('\n')
    f.write('===== END RSA PUBLIC KEY =====\n')
    f.write('\n===== BEGIN RSA PRIVATE KEY =====\n')
    f.write(str(private_key)[2:-1].strip('"\''))
    f.write('\n')
    f.write('===== END RSA PRIVATE KEY =====\n')
    f.write('\n===== BEGIN RSA SIGNATURE =====\n')
    f.write(str(signature)[2:-1].strip('"\''))
    f.write('\n')
    f.write('===== END RSA SIGNATURE =====\n')
    f.close()
    return

def is_padded(padding: int):
    return padding != 3

def check_padding(param_64):
    num_of_padding = len(param_64) % 3
    padding = 3 - num_of_padding
    return '', padding

def format64(public_key, private_key, signature):
    public_key_64 = str(public_key)
    private_key_64 = str(private_key)
    signature_64 = str(signature)

    end, padding = check_padding(public_key_64)
    
    if is_padded(padding):
        public_key_64 += 'A' * padding  # Ascii A
        end += '=' * padding         # Ascii =

    public_key_64 = public_key_64.encode()
    public_key_64 = b2a_base64(public_key_64, newline = False)

    if is_padded(padding):
        public_key_64 = public_key_64[:-padding]
        public_key_64 += end.encode()

    end, padding = check_padding(private_key_64)

    if is_padded(padding):
        private_key_64 += 'A' * padding
        end += '=' * padding

    private_key_64 = private_key_64.encode()
    private_key_64 = b2a_base64(private_key_64, newline = False)

    if is_padded(padding):
        private_key_64 = private_key_64[:-padding]
        private_key_64 += end.encode()

    end, padding = check_padding(signature_64)

    if is_padded(padding):
        signature_64 += 'A' * padding
        end += '=' * padding

    signature_64 = signature_64.encode()
    signature_64 = b2a_base64(signature_64, newline = False)
    
    if is_padded(padding):
        signature_64 = signature_64[:-padding]
        signature_64 += end.encode()

    return public_key_64, private_key_64, signature_64