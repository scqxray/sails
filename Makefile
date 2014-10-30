DIRS	= base crypto log net system

all:
	@for dir in $(DIRS); do make -C $$dir; echo; done
	@for dir in $(DIRS); do cp $$dir/*.a ./; echo "cp "$$dir static library; done 
	rm -f libsails.a
	@for commlib in *.a; do ar x $$commlib; done
	rm -r *.a
	ar cru libsails.a *.o
	rm -r *.o
	ranlib libsails.a


clean:
	@for dir in $(DIRS); do rm $$dir/*.o; rm $$dir/*.a; rm $$dir/*.d; done
	rm libsails.a
