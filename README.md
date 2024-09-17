# Dicionário de Frequência de Palavras
Trabalho da cadeira de Estrutura de Dados Avançada do curso de Ciência da Computação da UFC - Campus Quixadá, lecionada pelo professor Atílio Gomes Luiz no semestre 2024.1. Feito pelo aluno [Kauan Pablo](github.com/auanK).

## Descrição do Projeto
Este projeto consiste no desenvolvimento de uma aplicação em C++ para calcular a frequência de palavras em um texto, ordená-las alfabeticamente e realizar comparações de desempenho entre diferentes estruturas de dados usadas para armazenar essas palavras. O objetivo é analisar as métricas de desempenho dessas estruturas. Para mais detalhes, consulte o relatório disponível na pasta `docs`.

## Estruturas de Dados Implementadas
O projeto compara diferentes estruturas de dados:

- **Árvore AVL** (`dictionary_avl`)
- **Árvore Rubro-Negra** (`dictionary_rb`)
- **Tabela Hash com Encadeamento Exterior** (`dictionary_chained_hash`)
- **Tabela Hash com Endereçamento Aberto** (`dictionary_open_hash`)

## Dependências
É necessário ter o C++ (versão 17 ou superior) instalado, além da biblioteca ICU. A ICU (International Components for Unicode) é uma biblioteca que oferece suporte a manipulação de texto Unicode, essencial para comparação de strings neste projeto.

No Ubuntu, a biblioteca ICU pode ser instalada com o seguinte comando:

```bash
sudo apt-get install libicu-dev
```

## Como compilar e executar o projeto
Para compilar o projeto, execute o seguinte comando na raiz do projeto:

```bash
g++ src/main.cpp -std=c++17 -licuuc -licui18n
```

Após isso, dentro da pasta `src` será gerado um arquivo `a.out`. Para executar o projeto, utilize o seguinte comando dentro da pasta `src`:

```bash
./a.out <estrutura_de_dado> <arquivo_de_entrada.txt>
```

Onde:
- `<estrutura_de_dado>` é um dos valores: `dictionary_avl`, `dictionary_rb`, `dictionary_chained_hash` ou `dictionary_open_hash`.
- `<arquivo_de_entrada.txt>` é o nome do arquivo de texto, que deve estar na pasta `src/in`.

## Exemplo de execução
```bash
./src/a.out dictionary_avl biblia_sagrada_english.txt
```
Após essa execução a saída será um arquivo chamado dictionary_avl_biblia_sagrada_english.txt na pasta src/out.