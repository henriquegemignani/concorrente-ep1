Nomes: Gustavo Teixeira da Cunha Coelho   N.USP: 6797334
       Henrique Gemignani Passos Lima            6879634

O algoritmo roda em ciclos de 5 segundos, definido pelo valor da 
macro CICLO_TIME.

O relatório tem o seguinte formato:

- Listagem de cada ciclista e suas três velocidades na ordem (D, P, S).
- Listagem de cada minuto e ciclistas em cada kilometro até o final 
  da corrida.
- Listagem da chegada dos ciclistas nos checkpoints individuais em ordem 
  de tempo.
- Listagem do ranking das camisetas amarela, verde e branco/vermelhas.


-- Questões deixadas em aberto --

Atribuição de velocidades aleatórias:

A distribuição das velocidades aleatórias para os ciclistas é dada 
por uma distribuição uniforme, sendo um valor escolhido dessa 
distribuição para cada uma das 3 velocidades dos ciclistas, sendo 
feito isso para todos os ciclistas.

Critério de desempate:

O ranking de desempate é:

- A escolha das camisetas amarelas é desempatada pelo ID dos ciclistas.
- A escolha das camisetas verdes é desempatada pelas escolhas das 
  camisetas amarelas.
- A escolha das camisetas branco/vermelhas é desempatada pelas escolhas 
  das camisetas verdes.

O desempate é tal por causa da existência de dois quicksorts, primeiro 
para a ordenação para a distribuição das camisetas amarelas, depois das 
camisetas verdes e depois das camisetas branco/vermelhas.