from random import randrange
from math import gcd

def egcd(a, b):
    t0 = s1 = 0
    t1 = s0 = 1
    while True:
        if a == 0:
            break
        tmp_3 = b % a
        tmp_2 = a
        tmp_1 = b // a
        q = tmp_1
        b = tmp_2
        a = tmp_3

        tmp_2 = t0 - (q*t1)
        tmp_1 = t1
        t0 = tmp_1
        t1 = tmp_2

        tmp_2 = s0 - (q*s1)
        tmp_1 = s1
        s0 = tmp_1
        s1 = tmp_2
    return b, t0, s0

def prime(i: int, primes: list):
    for prime in primes:
        if not (i == prime or i % prime):   # composite
            return False
    primes.append(i)    # append list of N first primes
    return i

def generateFirstPrimes(num_of_primes: int):
    primes = []     # list of N first primes
    i, p = 2, 0
    while True:
        if prime(i, primes):
            p += 1
            if p == num_of_primes:
                return primes
        i += 1

def getPrimeNumber(bit_size, prime_list: list):
    #   For security purposes, the integers p and q should be chosen at random, and should be similar in magnitude 
    #   but differ in length by a few digits to make factoring harder.
    floor = pow(2, bit_size - 1) + 1
    ceil =  pow(2, bit_size) - 1

    while True:
        rand_bits = randrange(floor, ceil)
        for prime in prime_list:
            if rand_bits % prime == 0:  # if divisible, it isn't prime
                break
        else:
            return rand_bits

def isMillerRabin(prime, rounds: int):
    if prime % 2 == 0:
        return False

    s = t = prime - 1
    r = 0
    it_ok = False
    while s % 2 == 0:
        s = s >> 1  # shift 1-bit
        r = r + 1   
    
    for _ in range(0, rounds):
        it = randrange(2, prime)
        x = pow(it, s, prime)
        if x == 1:
            continue
        for i in range(0, r):
            k = pow(2, i)*s
            j = pow(it, k, prime)
            if(t == j):
                it_ok = True
                break
        if it_ok:   # iteration num prime 
            it_ok = False
            continue
        else:       # iteration num not prime 
            return False
    return True
    
def getPrime(bit_size, num_primes: int):
    first_primes = generateFirstPrimes(num_primes)
    miller_rabin_rounds = 40   # http://stackoverflow.com/questions/6325576/how-many-iterations-of-rabin-miller-should-i-use-for-cryptographic-safe-primes
    p_q = []
    while len(p_q) < 2:
        prime = getPrimeNumber(bit_size, first_primes)
        if isMillerRabin(prime, miller_rabin_rounds):
            p_q.append(prime)
        else:
            continue
    primes = (p_q[0], p_q[1])
    return primes

def getKeys(bit_size, num_primes: int, debug: bool):
    if debug:
        p, q = 61, 53
        print('P =', p)
        print('Q =', q)
    else:
        p, q = getPrime(bit_size, num_primes)
    n = p * q
    d_found = False
    totient = (p - 1) * (q - 1)
    totient = totient // gcd(p-1, q-1)  # lcm
    while not d_found:
        if debug:
            e = 17
            print('E =', e)
        else:
            e = randrange(1, totient)
        if gcd(totient, e) == 1:
            a, b, c = egcd(totient, e)
            i = (b*totient + c*e)
            if a == i:
                d = c%totient
                d_found = True
    public_key = (e, n)
    private_key = (d, n)

    return n, public_key, private_key