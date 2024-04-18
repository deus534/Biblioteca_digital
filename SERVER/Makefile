CC		=	gcc
CFLAGS	=	-c -Wall -g -fpack-struct -I$(HEADDIR)/
#CFLAGS	=	-c -Wall -g -fpack-struct
#CFLAGS	=	-c -Wall -g
LDFLAGS	=	-Wall -g -pthread

VERSION	=
CLASE	=	20
OBJDIR	=	obj
BINDIR	=	bin
SRCDIR	=	src
HEADDIR	=	headers
OBJS	=	$(OBJDIR)/main$(VERSION).o $(OBJDIR)/funciones$(VERSION).o
SOURCES	=	main$(VERSION).c
HEADERS	=	funciones$(VERSION).h
EJEC	=	ejec


$(BINDIR)/$(EJEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/$(EJEC)

new: clean $(BINDIR)/$(EJEC)

git:
	git commit -m "Clase $(CLASE) Version $(VERSION)"
	git push
