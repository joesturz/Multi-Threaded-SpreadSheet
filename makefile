
main: main.cc message.o parser.o ss_sock.o ss_db.o spreadsheet.o \
      ss_xml.o server.o lobby.o editing_session.o
	@ echo -n "Building main..."
	@ g++ -g -o main $^ -lboost_system -lpthread -lboost_regex
	@ echo "done"

mock_server: tests/mock_server
	@ echo "Running mock server..."
	@ ./tests/mock_server

parse_tests: tests/parse_tests.cc message.o parser.o
	@ echo -n "Building parse tests..."
	@ g++ -o tests/parse_tests $^ -lboost_regex
	@ echo "done"
	@ echo "Running parse tests..."
	@ ./tests/parse_tests

sock_tests: tests/sock_tests.cc ss_sock.o parser.o message.o
	@ echo -n "Building sock tests..."
	@ g++ -o tests/sock_tests $^ -lboost_system -lpthread -lboost_regex
	@ echo "done"
	@ echo "Running sock tests..."
	@ ./tests/sock_tests

ss_db_tests: tests/ss_db_tests.cc ss_db.o
	@ echo -n "Building ss db tests..."
	@ g++ -o tests/ss_db_tests $^
	@ echo "done"
	@ echo -n "Removing any existing ss_db.txt file..."
	@ rm -f ss_db.txt
	@ echo "done"
	@ echo "Running ss db tests..."
	@ ./tests/ss_db_tests

ss_tests: tests/ss_tests.cc spreadsheet.h exceptions.h spreadsheet.o ss_db.o ss_xml.o
	@ echo -n "Building ss tests..."
	@ g++ -o tests/ss_tests $^
	@ echo "done"
	@ echo -n "Removing any existing ss_db.txt file..."
	@ rm -f ss_db.txt
	@ echo "done"
	@ echo "Running ss tests..."
	@ ./tests/ss_tests

message.o: message.cc message.h parser.h exceptions.h
	@ echo -n "Building message.o..."
	@ g++ -c $<
	@ echo "done."

parser.o: parser.cc parser.h message.h exceptions.h
	@ echo -n "Building parser.o..."
	@ g++ -c $<
	@ echo "done."

ss_sock.o: ss_sock.cc ss_sock.h message.h
	@ echo -n "Building ss_sock.o..."
	@ g++ -c $<
	@ echo "done."

ss_db.o: ss_db.cc ss_db.h exceptions.h
	@ echo -n "Building ss_db.o..."
	@ g++ -c $<
	@ echo "done."

spreadsheet.o: spreadsheet.cc spreadsheet.h exceptions.h ss_xml.h ss_db.h
	@ echo -n "Building spreadsheet.o..."
	@ g++ -c $<
	@ echo "done."

ss_xml.o: ss_xml.cc ss_xml.h
	@ echo -n "Building ss_xml.o..."
	@ g++ -c $<
	@ echo "done."

server.o: server.cc server.h lobby.h ss_sock.h
	@ echo -n "Building server.o..."
	@ g++ -c $<
	@ echo "done."

lobby.o: lobby.cc lobby.h ss_sock.h spreadsheet.h editing_session.h
	@ echo -n "Building lobby.o..."
	@ g++ -c $<
	@ echo "done."

editing_session.o: editing_session.cc editing_session.h ss_sock.h spreadsheet.h
	@ echo -n "Building editing_session.o..."
	@ g++ -c $<
	@ echo "done."

tests/mock_server: tests/mock_server.cc ss_sock.h message.h \
                   ss_sock.o message.o parser.o
	@ echo -n "Building mock server..."
	@ g++ -o tests/mock_server $^ -lboost_system -lpthread -lboost_regex
	@ echo "done."

clean:
	@ rm -f *~ *.o *.out *.exe *.xml
	@ rm -f ss_db.txt
	@ rm -f tests/parse_tests
	@ rm -f tests/sock_tests
	@ rm -f tests/mock_server
	@ rm -f tests/ss_tests
	@ rm -f tests/ss_db_tests
	@ rm -f main