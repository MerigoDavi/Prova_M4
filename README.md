# Descrição da Atividade

## Estados do Sistema

**Inicialização**
- Configurado e inicializa (pinos, sensores e rede WiFi)
- Configura os pinos, inicializa as variáveis, conecta ao WiFi.

**Conexão WiFi**
- Tenta se conectar à rede WiFi.
- Verifica o status da conexão.

**Conexão ao Broker MQTT**
- Tenta se conectar ao broker MQTT.
- Verifica o status da conexão.

**Modo Noturno**
- Detecta se está escuro e entra no modo noturno, onde o LED amarelo pisca a cada segundo.

**Modo Convencional**
- Detecta se está claro e entra no modo convencional, alternando entre os LEDs verde, amarelo e vermelho.
- Alterna entre os LEDs verde (3 segundos), amarelo (2 segundos) e vermelho (5 segundos).

**Pressionamento do Botão**

- Detecta quando o botão é pressionado e implementa debounce.
- Conta o número de pressionamentos do botão.

**Abertura do Semáforo**
- Quando o semáforo está no estado fechado e o botão é pressionado 3 vezes, o semáforo abre 1 segundo.
- Muda o semáforo para verde.

**Envio de Requisição HTTP**
- Após o terceiro pressionamento do botão, o sistema envia uma requisição HTTP para um alerta.
- Envia uma requisição HTTP para http://www.google.com.br/.
  
**Desconexão e Reconexão WiFi**
- O sistema verifica se houve alguma desconexão da rede WiFi e tenta se reconectar.
