Um sistema completo para monitoramento de energia em tempo real, permitindo que você entenda seus gastos, identifique os aparelhos que mais consomem e tome decisões inteligentes para economizar na conta de luz.

Este projeto combina hardware customizado e um aplicativo mobile para oferecer uma solução de ponta a ponta. O coração do sistema é um ESP32 que lê dados de um sensor de corrente não-invasivo e os envia para um aplicativo, onde são traduzidos em informações claras e acionáveis sobre seu consumo e gastos em Reais (R$).

💡 Visão Geral do Projeto
Muitas vezes, a conta de luz chega como uma surpresa no fim do mês. O E-Mon foi criado para resolver esse problema, fornecendo um feedback instantâneo sobre seu consumo de energia.

Monitoramento em Tempo Real: Veja o consumo de energia da sua casa ou de um circuito específico em tempo real no seu celular.

Análise de Custos: O aplicativo converte o consumo em kWh para o custo real em Reais, com base na tarifa da sua concessionária de energia.

Hardware Aberto: O design da placa de circuito impresso (PCB) foi feito em KiCad e está totalmente disponível.

Firmware Robusto: Desenvolvido com ESP-IDF (framework nativo da Espressif) para máxima performance e controle do hardware.
⚙️  Arquitetura do Sistema
O sistema é dividido em três componentes principais:

Hardware (A Placa): Uma PCB customizada com um ESP32, um circuito de condicionamento de sinal para o sensor de corrente (SCT-013) e uma fonte de alimentação.

Firmware (A Inteligência Embarcada): O código que roda no ESP32, responsável por:

Ler os dados analógicos do sensor.

Calcular a corrente RMS, potência e energia consumida (kWh).

Disponibilizar os dados via Bluetooth ou Whatsapp.

Aplicativo Mobile (A Interface): Um app feito em MIT APP Inventor que:

Se conecta ao hardware via Bluetooth.

Exibe os dados de consumo em tempo real.

Permite ao usuário inserir o valor da tarifa (R$/kWh).

Calcula e exibe o custo acumulado em Reais.
