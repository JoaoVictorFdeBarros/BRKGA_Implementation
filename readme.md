## Instance Generator

Gera as instâncias de acordo com as definições do artigo:

### Tamanhos dos bins
- Para classes de 1 a 5 e 8 todos os bins possuem tamanho 100,100,100
- Para a classe 6 os bins possuem tamanho 10,10,10
- Para a classe 7 os bins possuem tamanho 40,40,40

### Ditribuição das caixas
- Para as classes de 1 a 5 cada indvíduo tem 60% de chance de pertencer à classe definida e 10% de pertencer a outra entre 1 e 5.
- Para as classes de 6 a 8 todos os indivíduos pertecem à classe definida

### Valores
Para ver os limites máximos e mínimos de cada classe consulte os tópicos 3.1 e 3.2 do artigo.

##  Bin

 ### Construtor
 Recebe as dimensoes (D) e define o container de 0,0,0 até Dx,Dy,Dz.

 ### Update

- Recebe a caixa a ser posicionada, já rotacionada, o EMS na qual será o ponto 0,0,0 da caixa será posicionado e os valores mínimos de volume e largura das caixas restantes

- Calcula o volume ocupado pela caixa

- Percorre o array de EMS até encontrar os EMS's que a caixa ocupa e os elimina pois serão quebrados em EMS's menores

- Cria 6 novos EMS's, os quais podem ou não ser validados, um em cada face da caixa posicionada

- Verifica se os novos EMS's não estão contidos em outro EMS e se suas dimensões são maiores que o volume e largura mínimos, em caso positivo, adiciona-se ele à lista de EMS's 

- É chamado pelo método Placement do PlacementProcedure

 ### Overlapped
Verifica se 2 EMS's estão sobrepostos

### Inscribed
Verifica se um EMS está inscrito em outro

### Eliminate
Remove um EMS do array de EMS's

### Load
Calcula a procentagem de uso do bin

### Get EMS's
Retorna o array de EMS's do bin

## Placement Procedure

### Construtor
- Recebe as instancias geradas as dimensões dos bins e a solução daquele inididuo (vetor com a ordem e a rotação de cada uma das caixas)
- constroi o BPS (Box Packing Sequence) com a primeira metade e o VBO (Vector of Box Orientations) com a segunda metade da solução

### Placement
- A partir dos valores de BPS, busca em cada um dos bins (em ordem) até encontrar um que caiba aquela caixa (usando o DFTRC) e vai para a próxima, caso não caiba em nenhum abre um novo bin
- Para a orientação da caixa, Verifica quais posições são possíveis no EMS e define a partir do VBO qual delas será escolhida

### DFTRC_2
- Recebe a caixa a ser posicionada e o bin, verifica entre todos os EMS's daquele bin, quais cabem a caixa, e dentre os que cabem, qual tem a maior distancia para o front top right corner (posição oposta de onde a 1 caixa do bin é colocada). 
- Se não tiver nenhum ele retorna 0,0,0 , 0,0,0 e será chamado novamente no metodo placement com outro bin. Caso nenhum bin caiba o método placement abre um novo bin

### Orient
- Recebe a caixa a ser orientada e a direção na qual deve ser posicionada e retorna as dimensões (altura, largura e profundidade) resultantes da rotação. 
- É chamado no método DFTRC_2 para verificar se a caixa cabe em um EMS e no método Placement para de fato posicionar ela.

### Select box orientation
- Recebe a caixa o valor do VBO para aquela caixa e o EMS onde a caixa será colocada.
- Verifica se aquela caixa cabe naquele EMS em cada uma das 6 rotações possiveis
- Multiplica o valor do VBO da caixa pela quantidade de caixas disponíveis, e usa arrendodamento (ceil) caso necessário, para definir a rotação da caixa e a retorna
- É chamado no método Placement para todas as caixas, a quantidade de vezes necessária até que encontre um bin que a caiba

### Fit in
- Recebe uma caixa e um EMS e retorna se a caixa cabe no EMS
- É chamado no método Select box orientation


### Elimination rule
- Recebe um vetor com as dimensões das caixas restantes, intera sobre elas e retorna um pair com o menor volume e menor dimensão dentre elas
- É chamado no Placement e passado ao método Update do Bin para eliminar EMS menores que essas medidas, pois nenhuma caixa iria caber neles

### Evaluate
- Avalia e retorna o fitness do individuo. O calculo é feito como: quantidade de bins abertos + fração de uso bin do menos usado.
- O algoritmo minimiza o fitness, então o primeiro critério é: quem usa menos bins é melhor, e para os que usam a mesma quantidade de bins o que possui o bin menos cheio mais vazio é melhor.

## BRKGA

### Construtor
- Inicializa o input boxes e o input bin dims com as instâncias geradas no instance generator.
- Recebe o número de gerações, idividuos em cada geração, elites em cada geração, mutantes em cada geração e probablilidade de herança do gene de elite na mutação.
- Define o numero de de genes como 2 vezes a quantidade de caixas (BPS e VBO para cada)
- inicializa o used bins e best fitness como -1

### Decoder
- Recebe uma solution (idividuo) e chama para ele o placement para simular seu posicionamento e o evaluate para avaliar a qualidade dele
- Retorna o valor do evaluate

### Calculate fitness
- Recebe toda a população de individuos da geração e chama o decoder para cada um dos individuos e cria a fitness list com o evaluate dos individuos

### Partition
- Ordena o fitness list (em ordem crescente - quanto menor, melhor)
- limpa os vetores de elites, non elites e eliet fitnesslist
- Coloca o n primeiros no vetor de elites (n = num elites)
- coloca o restante no vetor de não elites

### Crossover
- Recebe 2 inidividuos, um elite e um não elite
- Cria um novo individou (offspring)
- Para cada gene do novo individuo sorteia um número de 0 a 1, se for que a problabilidade de crossover de elite, o gene vem do individuo de elite, se for maior o gene vem do non elite
- Retorna o novo individuo

### Mating
- Recebe o vetor de elites e non elites
- Calcula o número de individuos que devem ser gerados na geração, descontanto elites e mutantes
- Chama o método Crossover em loop (uma vez para cada individuo da geração), passando como parametro um idividuo elite e um non elite sorteados aleatoriamente
- Retorna o vetor com os novos indivíduos.

### Mutants
- Cria uma matriz com a linhas sendo cada um dos novos individuos mutantes e as colunas sendo seus genes
- Sorteia um valor aleatório para cada campo da matriz (entre 0 e 1)
- Retorna a matriz com os inidividuos mutantes

### Fit
#### Inicialização
- Recebe como parametro o patience para early stop
- Cria a primeira geração de individuos com valores aleatórios
- Calcula o fitness dos individuos
- Pega o valor do melhor fitness e o individuo que o possui

#### Loop
- Inicialmente avalia se as condições de early stop são atendidas, se forem, retorna, se não, continua
- Chama o método partition para separar a geração anterior em elites e non elites
- Chama o método mating para fazer o crossover e criar novos individuos
- Chama o método mutants para criar novos individuos mutantes
- Cria uma matriz para representar a nova geração, cada linha é um indivíduo e cada coluna é um gene
- Adiciona na nova geração as elites, os descendentes e os mutantes
- Chama o método calculate fitness para a nova população
- Avalia se o melhor individuo da geração nova é melhor que o melhor da geração anterior, caso seja atualiza os valores de melhor fitness e melhor idividuo, também atualiza o valor da melhor geração, o qual é usando no early stop caso fique estagnado mais de N gerações

#### Final
- Atualiza a quantidade de bins utilizados 

## Main
Na função main são definidos os parametros iniciais, interpretadas as flags e instanciadas as caixas com o método generate instances e criado um objeto do tipo BRKGA e chamado seu método fit para iniciar o processamento, Ao final é mostrado o resultado

## Flags

- `-n <num>`: Número de caixas (padrão: 100)
- `-g <num>`: Número de gerações do BRKGA (padrão: 200)
- `-p <num>`: Tamanho da população (padrão: 30*n)
- `-e <num>`: Número de elites (padrão: 0.1*p)
- `-mut <num>`: Número de mutantes (padrão: 0.15*p)
- `-prob <float>`: Probabilidade de crossover elite (padrão: 0.7)
- `-patience <num>`: Paciência para early stopping (padrão: 10)
-  `-t <num>`: define o tipo das instancias [1-8] (ver artigo)
- `-h`: Ajuda.


## Paralelização
- Cada indivíduo de uma geração será processado paralelamente
- Possibilita mais inidiviuos em uma mesma geração
- Ao fim de cada geração os dados são avaliados novamente na CPU, instanciada a nova geração, que vai para a GPU novamente
- Métodos que devem ser alterados 
- Bin:: Update
- Placement_Procedure:: Placement
- Métodos auxiliares