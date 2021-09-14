############################# Makefile ##########################
CC = g++
CTARGET = src/main.cpp
EXECNAME = aes

compile: 
	$(CC) -o $(EXECNAME) $(CTARGET)
	@echo "Execute using ./aes [-c, -d] <key> -f <source_file> -o <destination_file> [-ecb, -ctr] <num_rounds>"

run_ecb1:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ecb1.ppm -ecb 1
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ecb1.ppm -o tests/d_penguin_ecb1.ppm -ecb 1

run_ecb5:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ecb5.ppm -ecb 5
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ecb5.ppm -o tests/d_penguin_ecb5.ppm -ecb 5

run_ecb9:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ecb9.ppm -ecb 9
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ecb9.ppm -o tests/d_penguin_ecb9.ppm -ecb 9

run_ecb13:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ecb13.ppm -ecb 13
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ecb13.ppm -o tests/d_penguin_ecb13.ppm -ecb 13

run_ctr1:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ctr1.ppm -ctr 1
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ctr1.ppm -o tests/d_penguin_ctr1.ppm -ctr 1

run_ctr5:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ctr5.ppm -ctr 5
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ctr5.ppm -o tests/d_penguin_ctr5.ppm -ctr 5

run_ctr9:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ctr9.ppm -ctr 9
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ctr9.ppm -o tests/d_penguin_ctr9.ppm -ctr 9

run_ctr13:
	@echo "Encrypting!"
	@sleep 2
	@./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/penguin.ppm -o tests/c_penguin_ctr13.ppm -ctr 13
	@sleep 5
	@echo "Decrypting!"
	@sleep 2
	@./aes -d a5b90c1a2b5e3a014253698745214523 -f tests/c_penguin_ctr13.ppm -o tests/d_penguin_ctr13.ppm -ctr 13

clean: $(RM) $(aes)