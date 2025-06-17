# FingEmulator

Este proyecto fue desarrollado en **Microsoft Visual Studio Community 2019**.
Incluye tres partes:
- **Fingemulator** que es la plataforma principal de emulacion. 
- **FingemulatorAPI** donde se implementa la API de entrada salida. 
- **TestAPI** como ejemplo de cómo usar **FingEmulatorAPI**. 

## Dependencias
Incluye todas las dependencias necesarias, ya que son de **libre distribución**.  
Para compilarlo, solo se necesita:  
1. Descargar el proyecto.  
2. Compilar la solución en Visual Studio en modo Release x64.

Asegurarse de compilar los proyectos en modo release x64 y en el orden el que vienen, sino puede haber un error al intentar compilar TestAPI por no haber compilado **FingemulatorAPI** primero. 
Si se encuentra algún problema con las dependencias, estas están disponibles en la carpeta **Fingemulator/deps** para poder integrarse manualmente

### Cómo ejecutar TestAPI
1. Compilar el proyecto.  
2. Arrastrar una **ROM** con mapper 0 al ejecutable compilado para correrlo.