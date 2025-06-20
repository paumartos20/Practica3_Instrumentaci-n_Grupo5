# Practica3_Instrumentaci-n_Grupo5

1. Introducción
El presente documento describe el desarrollo de un sistema automatizado de monitorización y control climático utilizando una placa Arduino. Este sistema permite medir variables ambientales clave como temperatura, humedad, luminosidad, velocidad del viento y calidad del aire. Además, incluye funcionalidades de actuación mediante un servomotor, visualización en pantalla LCD, y control remoto mediante un mando IR. La información se muestra tanto en un display como en el monitor serial, y se emplea un sistema de autodiagnóstico para garantizar lecturas fiables.
2. Objetivos
- Medir temperatura y humedad mediante un sensor DHT22.
- Medir la luminosidad ambiental, la velocidad del viento y la calidad del aire con sensores analógicos.
- Visualizar la información en una pantalla LCD I2C.
- Actuar sobre una válvula (servo) según la temperatura leída.
- Ajustar dinámicamente el umbral de temperatura mediante un control remoto IR.
- Implementar un sistema de promedio móvil para suavizar las lecturas.
- Controlar el nivel de iluminación con un registro de desplazamiento 74HC595 y LEDs.
3. Materiales Utilizados
Componente	Descripción
Arduino Uno	Placa principal del sistema
DHT22	Sensor digital de temperatura y humedad
Sensor LDR	Sensor analógico de luz
Sensores analógicos	Para viento y calidad del aire
Servo motor	Actúa como válvula de control térmico
Pantalla LCD I2C (20x4)	Muestra información ambiental
Registro 74HC595	Controla 8 LEDs para indicar nivel de luz
Control remoto IR + receptor	Permite cambiar el 'setpoint' de temperatura
Resistencias, cables, protoboard	Interconexión de componentes
4. Descripción del Funcionamiento
4.1 Sensado de Variables Ambientales
- Temperatura y humedad: Se obtienen del sensor DHT22, cuya señal se filtra mediante un promedio móvil para reducir el ruido.
- Luminosidad (LDR): Se mapea de 0 a 100% y se filtra también con un promedio móvil.
- Velocidad del viento y calidad del aire: Se simulan mediante entradas analógicas, con mapeo a valores útiles (km/h y % respectivamente).
4.2 Control Térmico con Servo
El sistema compara la temperatura promedio con un setpoint ajustable. Si la temperatura excede el umbral superior, el servo abre una válvula (posición 180°). Si está por debajo del umbral inferior, la cierra (0°). En el rango de histéresis (±2 °C), se posiciona en modo neutral (90°), considerado como “zona muerta”.
4.3 Visualización de Datos
La pantalla LCD de 4 líneas muestra:
- Temperatura actual y setpoint.
- Humedad y luminosidad.
- Velocidad del viento y calidad del aire.
- Estado del sistema (calentando, enfriando, zona muerta).
4.4 Control por Infrarrojos
El receptor IR interpreta señales del mando para:
- Subir el setpoint (+1 °C).
- Bajar el setpoint (-1 °C).
- Reiniciar el setpoint a 25 °C.
4.5 Iluminación con LEDs
El sistema utiliza un registro de desplazamiento 74HC595 para encender de 0 a 8 LEDs según el nivel de luminosidad. Esta lógica representa gráficamente la intensidad de luz ambiente.
5. Funcionalidades Avanzadas
5.1 Autodiagnóstico del Sensor de Temperatura
Si el sensor DHT22 falla (valores NaN o fuera de rango razonable), se muestra un mensaje de error en pantalla y se utiliza el último valor válido para evitar errores en el control del servo.
5.2 Promedios Móviles
Se implementa un promedio móvil de 5 muestras para las lecturas de temperatura y luz, lo que estabiliza las medidas y evita fluctuaciones bruscas.
6. Resultados Esperados
El sistema responde dinámicamente a los cambios en las condiciones ambientales, adaptando la apertura de la válvula de control térmico según la temperatura. Permite al usuario ajustar el comportamiento del sistema de forma remota y visualiza la información de forma clara. Se prevé que el sistema opere de forma autónoma con fiabilidad en condiciones normales de laboratorio o interior.
7. Conclusión
Se ha logrado diseñar e implementar un sistema funcional de monitorización climática con capacidades de sensado, visualización, actuación y control remoto. Este proyecto demuestra cómo integrar múltiples sensores y actuadores en una arquitectura coherente basada en Arduino, cumpliendo con criterios de estabilidad, interactividad y facilidad de uso.
