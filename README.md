# raycaster
Engine primitiva para jogos em primeira pessoa 2.5D usando um ray caster de array. A engine foi escrita em C, utilizando SDL como a camada de mídia. O projeto foi feito com fins educativos.
  
Caso queira compilar o código, é necessario ter a biblioteca do SDL2 baixada.  
O programa foi compilado com o GCC usando os seguintes argumentos:  
```gcc main.c $(pkg-config --cflags --libs sdl2) -lm ```

No repositório estão dois executáveis. O executável  ```game.exe``` mostra a visão do jogo em si, e o executável ```vision_map.exe``` mostra como os raios interagem com o mapa para criar a visão.