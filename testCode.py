import test
import time

def main():
	test.call()
	count = 0
	while 1:
		test.update(count)
		count += 1
		time.sleep(0.1)
		
main()
