CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
TARGET = BRKGA
SOURCES = main.cpp BinPacking3D.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = BinPacking3D.hpp

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linkando $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilação concluída com sucesso!"

%.o: %.cpp $(HEADERS)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos de compilação..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Limpeza concluída!"

run: $(TARGET)
	@echo "Executando $(TARGET) com parâmetros padrão..."
	./$(TARGET)

test: $(TARGET)
	@echo "Executando teste com parâmetros pequenos..."
	./$(TARGET) -n 10 -m 5 -V 50,50,50 -g 50 -p 30 -e 10 -mut 10

verbose: $(TARGET)
	@echo "Executando em modo verboso..."
	./$(TARGET) -n 5 -m 3 -V 30,30,30 -g 20 -p 20 -e 5 -mut 5 -v

help:
	@echo "Makefile para o projeto Bin Packing 3D"
	@echo ""
	@echo "Alvos disponíveis:"
	@echo "  all      - Compila o projeto (padrão)"
	@echo "  clean    - Remove arquivos de compilação"
	@echo "  run      - Compila e executa com parâmetros padrão"
	@echo "  test     - Executa teste com parâmetros pequenos"
	@echo "  verbose  - Executa em modo verboso com instância pequena"
	@echo "  help     - Mostra esta ajuda"
	@echo ""
	@echo "Variáveis:"
	@echo "  CXX      = $(CXX)"
	@echo "  CXXFLAGS = $(CXXFLAGS)"
	@echo "  TARGET   = $(TARGET)"

install-deps:
	@echo "Verificando dependências..."
	@which g++ > /dev/null || (echo "g++ não encontrado. Instale com: sudo apt-get install g++" && exit 1)
	@echo "Todas as dependências estão disponíveis!"

