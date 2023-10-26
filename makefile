all: email_sender


email_sender: email_sender.c
	gcc -o email_sender email_sender.c
	
clean:
	rm email_sender
