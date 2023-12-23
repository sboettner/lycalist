OBJS=main.o songmodel.o lyricedit.o melodyedit.o chordchooser.o

lycalist: $(OBJS)
	g++ -o lycalist $(OBJS) `pkg-config gtkmm-3.0 --libs`

$(OBJS): %.o: %.cc songmodel.h
	g++ -g -std=c++17 -c -o $@ $< `pkg-config gtkmm-3.0 --cflags`
