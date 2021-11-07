from binascii import b2a_base64

def gen64file(public_key, private_key, signature):
    print('Generating file `' + '\x1b[0;33;40m' + 'out.md' + '\x1b[0m' + '` containing public key, private key and signature in BASE64.')
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

def format64(public_key, private_key, signature, n):
    public_key_64 = str(public_key)
    private_key_64 = str(private_key) + str(n)
    
    public_key_64 = public_key_64.encode()
    public_key_64 = b2a_base64(public_key_64, newline = False)

    private_key_64 = private_key_64.encode()
    private_key_64 = b2a_base64(private_key_64, newline = False)

    signature_64 = b2a_base64(signature, newline = False)

    signature_size = len(signature_64)
    for i in range(signature_size):
        if signature_64[i] != 65:
            save_i = i
            break

    return public_key_64, private_key_64, signature_64[save_i:signature_size]