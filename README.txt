Nomes: Gustavo Teixeira da Cunha Coelho   N.USP: 6797334
       Henrique Gemignani Passos Lima            6879634

O algoritmo roda em ciclos de 5 segundos, definido pelo valor da 
macro CICLO_TIME.

O relat�rio tem o seguinte formato:

- Listagem de cada ciclista e suas tr�s velocidades na ordem (D, P, S).
- Listagem de cada minuto e ciclistas em cada kilometro at� o final 
  da corrida.
- Listagem da chegada dos ciclistas nos checkpoints individuais em ordem 
  de tempo.
- Listagem do ranking das camisetas amarela, verde e branco/vermelhas.


-- Quest�es deixadas em aberto --

Atribui��o de velocidades aleat�rias:

A distribui��o das velocidades aleat�rias para os ciclistas � dada 
por uma distribui��o uniforme, sendo um valor escolhido dessa 
distribui��o para cada uma das 3 velocidades dos ciclistas, sendo 
feito isso para todos os ciclistas.

Crit�rio de desempate:

O ranking de desempate �:

- A escolha das camisetas amarelas � desempatada pelo ID dos ciclistas.
- A escolha das camisetas verdes � desempatada pelas escolhas das 
  camisetas amarelas.
- A escolha das camisetas branco/vermelhas � desempatada pelas escolhas 
  das camisetas verdes.

O desempate � tal por causa da exist�ncia de dois quicksorts, primeiro 
para a ordena��o para a distribui��o das camisetas amarelas, depois das 
camisetas verdes e depois das camisetas branco/vermelhas.