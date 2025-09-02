#Paste the data from the json file here
dictionary = 

password_list = [1 for i in range(48)]

passwords_cracked = 0

for out in dictionary.keys():
    global passwords_cracked
    n = dictionary[out]['n']
    e = 65537
    c = dictionary[out]['c']
    secret1 = dictionary[out]['secret1']
    secret2 = dictionary[out]['secret2']
    index = dictionary[out]['index'] - 1
    
    k_approx = floor(n^(1/4))
    found = False
    for delta in range(0, 1001):
        k_val = k_approx + delta
        R.<a> = PolynomialRing(ZZ)
        poly = secret2 * a^8 + (k_val^4 + secret1*secret2 - n) * a^4 + secret1 * k_val^4
        roots = poly.roots()
        for a_val, times in roots:
            if a_val > 0:
                p_candidate = a_val^4 + secret1
                if n % p_candidate == 0:
                    p = p_candidate
                    q = n // p
                    found = True
                    break
        if found:
            break

    if found:
        phi = (p-1)*(q-1)
        d = inverse_mod(e, phi)
        m = pow(c, d, n)
        password = int(m).to_bytes((int(m).bit_length()+7)//8, 'big')
        password_list[index] = password.decode()
        passwords_cracked += 1
        print(f'Passwords cracked: {passwords_cracked}')
    else:
        print("Failed")

print('\n'.join(password_list))