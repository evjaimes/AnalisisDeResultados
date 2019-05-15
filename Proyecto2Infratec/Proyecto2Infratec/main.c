// ISIS-1304 - FUNDAMENTOS DE INFRAESTRUCTURA TECNOLOGICA - PROYECTO 2 - 201910
// EL GRUPO DEBE SER DESARROLLADO EN GRUPOS DE A 3 PERSONAS MAXIMO
//
// DESARROLLADO POR:
// Nombre - C√≥digo
// Nombre - C√≥digo
// Nombre - C√≥digo

#define _CRT_SECURE_NO_DEPRECATE 
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

// La representacion de la imagen
typedef struct img
{
	int ancho;
	int alto;
	unsigned char *informacion;
} Imagen;


// Funci√≥n que carga el bmp en la estructura Imagen
void cargarBMP24(Imagen * imagen, char * nomArchivoEntrada);

// Funci√≥n que guarda el contenido de la estructura imagen en un archivo binario
void guardarBMP24(Imagen * imagen, char * nomArchivoSalida);

//Funci√≥n que inserta un mensaje en la imagen usando n bits por Byte
void insertarMensaje(Imagen * img, unsigned char mensaje[], int n);

//Funci√≥n que lee un mensaje de una imagen dando la longitud del mensaje y el numero de bits por byte usados
void leerMensaje(Imagen * img, unsigned char msg[], int l, int n);

//Funci√≥n que saca n bits de una secuencia de caracteres a partir de una posici√≥n dada
unsigned char sacarNbits(unsigned char secuencia[], int bitpos, int n);


unsigned char salvarNBits(int n);


unsigned char matarNBits(int n);




// Programa principal
// NO MODIFICAR
int main(int argc, char* argv[]) {

	Imagen *img = (Imagen *)malloc(sizeof(Imagen));
	char msg[10000] = "";
	char op, temp;
	int num, l, n;
	char nomArch[256] = "";

	printf("\nISIS-1304 - FUNDAMENTOS DE INFRAESTRUCTURA TECNOLOGICA - PROYECTO 2\n");

	if (argc != 5)
	{
		printf("Se ingreso un numero incorrecto de argumentos (%d) o se ingresaron de forma erronea... :(\n", argc);
		system("pause");
		return -1;
	}

	strcat(nomArch, argv[1]); //Se concatena el nombre del archivo en su variable respectiva
	op = argv[2][0]; //Se extrae la opci√≥n a realizar
	sscanf(argv[3], "%d", &num); //Se formatea el n√∫mero de bits por Byte para leer o insertar

	if (op == 'w') {
		strcat(msg, argv[4]); //Se concatena la cadena a escribir en su variable, si se insertar√° un mensaje
	}
	else if (op == 'r') {
		sscanf(argv[4], "%d", &l); //Se formatea en l la longitud a leer de caracteres, si se leer√° un mensaje
	}
	else {
		printf("Se ingreso una opcion invalida para el programa... :(\n");
		system("pause");
		return -1;
	}

	printf("Se ingresaron los %d argumentos correctamente!\n", argc);

	// Cargar los datos
	cargarBMP24(img, nomArch);

	if (op == 'w') {
		printf("\nMODO INSERTAR MENSAJE\n\n");
		printf(" - Longitud mensaje: %d bytes\n", strlen(msg));
		//msg[strlen(msg)] = '\0';

		printf(" - Insertando mensaje . . .\n");
		insertarMensaje(img, msg, num);
		printf(" - Insertado correctamente!\n");

		strcat(nomArch, "generado.bmp");
		guardarBMP24(img, nomArch);

		printf(" - Mensaje insertado y guardado exitosamente en el archivo/ruta '%s'! :D\n\n", nomArch);
	}
	else if (op == 'r') {
		printf("\nMODO LEER MENSAJE\n\n");

		for (int i = 0; i < l; i++) {
			msg[i] = 0;
		}
		msg[l] = 0;

		leerMensaje(img, msg, l, num);

		printf(" - Mensaje obtenido exitosamente! El mensaje es:\n\n\t%s\n\n", msg);
	}

	system("pause");
	return 0;
}

/**
* Inserta un mensaje, de a n bits por componente de color, en la imagen apuntada por img
* par√°metro img: Apuntador a una imagen en cuyos pixeles se almacenar√° el mensaje.
* par√°metro mensaje: Apuntador a una cadena de caracteres con el mensaje.
* par√°metro n: Cantidad de bits del mensaje que se almacenar√°n en cada componente de color de cada pixel. 0 < n <= 8.
*/
// DESARROLLAR EN ENSAMBLADOR, *NO* SE PUEDEN USAR NOMBRES SIMBOLICOS
void insertarMensaje(Imagen * img, unsigned char mensaje[], int n) {
	
	//Tomamos el mensaje a encriptar y le sacamos el length
	//Multiplicamos el length por (tamaÒo en bytes un elemento del mensaje*8) <-- seria en bits practicamente.
	int bits = strlen(mensaje)*(sizeof mensaje[1]) * 8;

	//Este short es el que nos indicara en cuantos bits 'partidos' o sacados vamos de los bits del mensaje a encriptar.
	short bitsSacados = 0;

	//Bueno tengo la sensacion que aquÌ hay ciclo infinito
	int i = 0;
	while (bits > 0)
	{
		//El la primera parte lo que hacemos es crear la m·scara y la aplicamos sobre la informaciÛn de la img para poder dejar los bits
		//m·s significativos protegidos y los bits a asignar en ceros para poder insertarlos
		//Luego, para la parte antes de la disyunciÛn creamos el byte que contiene los pedazos de n bits del mensaje.
		//Finalmente generamos la disyunciÛn entre el byte de la imagen y el byte que contiene n bits de la imagen, al generar
		//La disyunciÛn nos genera el byte con la informaciÛn insertada.

		char byteAMeter = (salvarNBits(8 - n) & img->informacion[i]) | sacarNbits(mensaje, bitsSacados, n);
		img->informacion[i++] = byteAMeter;
		bits -= n;

		bitsSacados += n;
	}


	__asm {

		//Tengamos en cuenta que manejaremos stack
		//Entra n en pos x
		//Entra mensaje en pos x hasta lo que de basado en el tamaÒo del mensaje en bytes
		//entra el apuntador a la imagen que tiene una equivalencia de 3 bytes (tres espacios distintos en la pila)
		//entra eip que es retorno
		//segun el monitor push ebp y mov ebp, esp entran autom·tico (apuntador a la pila)

		sub esp, 12
		mov[ebp-4], 0 //tomar este valor es a lil dificil
		mov [ebp-8], 0	//bitssacados=0
		mov[ebp-12], 0 //i=0

		while:
		
		mov eax, [ebp-4] //muevo a eax el valor de bits
		cmp eax, 0 //veo si bits es igual a 0
		je finuno //si es asi termino el ciclo


		//Llamados y demas se veran pronto

		




	}
}

/**
* Extrae un mensaje de tamanio l, guardado de a n bits por componente de color, de la imagen apuntada por img
* par√°metro img: Apuntador a una imagen que tiene almacenado el mensaje en sus pixeles.
* par√°metro msg: Apuntador a una cadena de caracteres donde se depositar√° el mensaje.
* par√°metro l: Tamanio en bytes del mensaje almacenado en la imagen.
* par√°metro n: Cantidad de bits del mensaje que se almacenan en cada componente de color de cada pixel. 0 < n <= 8.
*/
// DESARROLLAR EN ENSAMBLADOR, SE PUEDEN USAR NOMBRES SIMBOLICOS
void leerMensaje(Imagen * img, unsigned char msg[], int l, int n) {
	
	//Permite el avance sobre el areglo que contiene la info de la imagen
	int i = 0;
	//Permite el acceso a el arreglo en donde se guarda el mensaje
	int j = 0;
	//Indica el numero de bits que sobraron del otro byte, los cuales deben ser tomados en cuenta para construir otro byte del mensaje
	int nosFaltan = 0;

	//Comienza a iterar sobre la imagen basado en el numero de bytes que tenga el mensaje
	while (l)
	{
		//Centinela que nos permite saber cu·ntos bits hemos leido o tomado hasta ahora
		int h = 8;
		char ByteAInsertar = 0;

		if (n == 1 || n == 2 || n == 4 || n == 8)
			//Funciona para el 1 y pares menos 6.
		{
			while (h > 0) {
				//Toma un byte de la imagen y le aplica la mascara para tomar los bits menos significativos del byte por el n de insertados
				char good = img->informacion[i++] & matarNBits(n);
				//realizamos corrimiento para permitir la entrada de los otros bytes
				ByteAInsertar <<= n;
				//disyuncion entre el byte y los datos (good) encontrados
				ByteAInsertar = (ByteAInsertar | good);
				//Resta el numero de bits ya revisados
				h -= n;
				//pos += n;
			}

			if (h == 0) {
				//Inserta el byte encontrado en el arreglo de bytes del mensaje
				msg[j] = ByteAInsertar;
			}
		}
		//CASOS IMPARES Y 6
		else {
			while (h > 0) {
				if (nosFaltan != 0) {
					//En caso de que tengamos bits sobrantes de una extracciÛn lo ponemos al inicio de el siguiente byte
					char b = img->informacion[i++] & matarNBits(nosFaltan);

					ByteAInsertar = ByteAInsertar | b;
					//Restamos el numero de bits que ya agregamos
					h -= nosFaltan;
					nosFaltan = 0;

				}

				//Revisa los bits extraidos y va extrayendo de los siguientes, si no cumple la condiciÛn es porque debemos tomar un numero distinto de bits para completar un byte
				if (h - n >= 0) {
					//Toma el byte y aplica mascara
					char b = img->informacion[i++] & matarNBits(n);

					//Hace corrimiento de n sobre el byte para poder meter la info
					ByteAInsertar <<= n;
					//disyuncion
					ByteAInsertar = (ByteAInsertar | b);
					//resta los bits conseguidos
					h -= n;
				}
				else {
					//Aqui entra en el caso de que tengamos un numero de bits diferentes a n para completar un byte del mensaje
								//Hacemos corrimiento sobre el byte el numero de bits que nos faltan para completarlo
					ByteAInsertar <<= h;
					//Aplicamos sobre el siguiente y sacamos los bits con mask
					char b = img->informacion[i] & matarNBits(n);
					//Realizamos corrimiento sobre el byte anterior de forma que nos quede el numero de bits que requerimos para completar el byte del mensaje
					b >>= n - h;
					//disyuncion, unimos y logramos juntar los bits que tenemos con los juntados
					ByteAInsertar = (ByteAInsertar | b);
					//restamos los que ya leÌamos
					h -= n;
					//guardamos el numero de bits que sobraron y no usamos para meterlo de primeras en otro byte
					nosFaltan = h * -1;

				}
				//insertamos el byte en el arreglo de bytes del mensaje
				if (h <= 0) {
					msg[j] = ByteAInsertar;

				}

			}
		}
		//aumentamos pos en el arreglo de bytes del mensaje
		j++;
		//disminuimos el numero de bytes para recorrer
		l--;
	}
	
	

	__asm {

	}
}

/**
* Extrae n bits a partir del bit que se encuentra en la posici√≥n bitpos en la secuencia de bytes que
* se pasan como par√°metro
* par√°metro secuencia: Apuntador a una secuencia de bytes.
* par√°metro n: Cantidad de bits que se desea extraer. 0 < n <= 8.
* par√°metro bitpos: Posici√≥n del bit desde donde se extraer√°n los bits. 0 <= n < 8*longitud de la secuencia
* retorno: Los n bits solicitados almacenados en los bits menos significativos de un unsigned char
*/
// DESARROLLAR EN ENSAMBLADOR, SE PUEDEN USAR NOMBRES SIMBOLICOS
unsigned char sacarNbits(unsigned char secuencia[], int bitpos, int n) {
	// DESARROLLO OPCIONAL: Puede ser √∫til para el desarrollo de los procedimientos obligatorios.
	
	char a = secuencia[bitpos / 8];

	if (bitpos % 8 > 8 - n) {
		//La idea para continuar ac· es tomar los bits faltantes de la siguiente parte del arreglo
		//Hablamos antes sobre limpiar el siguiente del arreglo dejando los bits que necesitamos y con un or al otro y sirve
		char b = secuencia[(bitpos + 8) / 8];

		b = b >> 8 - ((bitpos % 8) + n) % 8;

		a = a & matarNBits(8 - bitpos % 8);
		a = a << (n - (8 - bitpos % 8));

		//a = (a & (salvarNBits(8-(bitpos%8)) >>8-(bitpos%8)));

		return a | b;

	}

	//Primera respuesta del ternario funciona para 0<n<6
	return (bitpos % 8 == 0) ? (a& salvarNBits(n)) >> 8 - n : (a >> (8 - (bitpos % 8) - n)) & matarNBits(n);

	
	
	__asm {

	}
}



//METODOS AUXILIARES


/**
 * Guarda los bits m·s significativos de un byte.
 * @param n numero de bytes significativos a guardar. Genera mascara, permite vaciar los bits menos significativos.
 * @return mascara que guarda los bits mas significativos
 */
unsigned char salvarNBits(int n)
{
	int i;
	unsigned char mask = 0;

	for (int i = 0; i < n; i++) {
		//Le sumamos a la m·scara 1
		mask++;
		//Hacemos corrimiento de 1 bit a izquierda
		mask <<= 1;
	}

	//No existe k como tal pero me imagino que es completarByte
	int completarByte = 8 - n - 1;
	if (completarByte > 0)
		mask <<= completarByte;
	else
		mask++;
	return mask;


	__asm {


	}


}

/**
 * Guarda los bits menos significativos de un byte.
 * @param n numero de bits menos asingificativos a guardar
 * @return mascara con los bits menos significativos.
 */
unsigned char matarNBits(int n)
{

	unsigned char mask = 0;
	for (int i = 1; i < n; i++)
	{
		//Le sumamos a la m·scara 1
		mask++;
		//Hacemos corrimiento de 1 bit a izquierda
		mask <<= 1;
	}
	mask++;
	return mask;


	__asm {

	}
}






// Lee un archivo en formato BMP y lo almacena en la estructura img
// NO MODIFICAR
void cargarBMP24(Imagen * imagen, char * nomArchivoEntrada) {
	// bmpDataOffset almacena la posici√≥n inicial de los datos de la imagen. Las otras almacenan el alto y el ancho
	// en pixeles respectivamente
	int bmpDataOffset, bmpHeight, bmpWidth;
	int y;
	int x;
	int	residuo;

	FILE *bitmapFile;
	bitmapFile = fopen(nomArchivoEntrada, "rb");
	if (bitmapFile == NULL) {
		printf("No ha sido posible cargar el archivo: %s\n", nomArchivoEntrada);
		exit(-1);
	}

	fseek(bitmapFile, 10, SEEK_SET); // 10 es la posici√≥n del campo "Bitmap Data Offset" del bmp	
	fread(&bmpDataOffset, sizeof(int), 1, bitmapFile);

	fseek(bitmapFile, 18, SEEK_SET); // 18 es la posici√≥n del campo "height" del bmp
	fread(&bmpWidth, sizeof(int), 1, bitmapFile);
	bmpWidth = bmpWidth * 3;

	fseek(bitmapFile, 22, SEEK_SET); // 22 es la posici√≥n del campo "width" del bmp
	fread(&bmpHeight, sizeof(int), 1, bitmapFile);

	residuo = (4 - (bmpWidth) % 4) & 3; // Se debe calcular los bits residuales del bmp, que surjen al almacenar en palabras de 32 bits

	imagen->ancho = bmpWidth;
	imagen->alto = bmpHeight;
	imagen->informacion = (unsigned char *)calloc(bmpWidth * bmpHeight, sizeof(unsigned char));

	fseek(bitmapFile, bmpDataOffset, SEEK_SET); // Se ubica el puntero del archivo al comienzo de los datos

	for (y = 0; y < bmpHeight; y++) {
		for (x = 0; x < bmpWidth; x++) {
			int pos = y * bmpWidth + x;
			fread(&imagen->informacion[pos], sizeof(unsigned char), 1, bitmapFile);
		}
		fseek(bitmapFile, residuo, SEEK_CUR); // Se omite el residuo en los datos
	}
	fclose(bitmapFile);
}

// Esta funci√≥n se encarga de guardar una estructura de Imagen con formato de 24 bits (formato destino) en un archivo binario
// con formato BMP de Windows.
// NO MODIFICAR
void guardarBMP24(Imagen * imagen, char * nomArchivoSalida) {
	unsigned char bfType[2];
	unsigned int bfSize, bfReserved, bfOffBits, biSize, biWidth, biHeight, biCompression, biSizeImage, biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImportant;
	unsigned short biPlanes, biBitCount;
	FILE * archivoSalida;
	int y, x;
	int relleno = 0;

	int residuo = (4 - (imagen->ancho) % 4) & 3; // Se debe calcular los bits residuales del bmp, que quedan al forzar en palabras de 32 bits


	bfType[2];       // Tipo de Bitmap
	bfType[0] = 'B';
	bfType[1] = 'M';
	bfSize = 54 + imagen->alto * ((imagen->ancho) / 3) * sizeof(unsigned char);       // Tamanio total del archivo en bytes
	bfReserved = 0;   // Reservado para uso no especificados
	bfOffBits = 54;    // Tamanio total del encabezado
	biSize = 40;      // Tamanio del encabezado de informacion del bitmap	
	biWidth = (imagen->ancho) / 3;     // Ancho en pixeles del bitmap	
	biHeight = imagen->alto;    // Alto en pixeles del bitmap	
	biPlanes = 1;    // Numero de planos	
	biBitCount = 24;  // Bits por pixel (1,4,8,16,24 or 32)	
	biCompression = 0;   // Tipo de compresion
	biSizeImage = imagen->alto * imagen->ancho;   // Tamanio de la imagen (sin ecabezado) en bits
	biXPelsPerMeter = 2835; // Resolucion del display objetivo en coordenada x
	biYPelsPerMeter = 2835; // Resolucion del display objetivo en coordenada y
	biClrUsed = 0;       // Numero de colores usados (solo para bitmaps con paleta)	
	biClrImportant = 0;  // Numero de colores importantes (solo para bitmaps con paleta)	

	archivoSalida = fopen(nomArchivoSalida, "w+b"); // Archivo donde se va a escribir el bitmap
	if (archivoSalida == 0) {
		printf("No ha sido posible crear el archivo: %s\n", nomArchivoSalida);
		exit(-1);
	}

	fwrite(bfType, sizeof(char), 2, archivoSalida); // Se debe escribir todo el encabezado en el archivo. En total 54 bytes.
	fwrite(&bfSize, sizeof(int), 1, archivoSalida);
	fwrite(&bfReserved, sizeof(int), 1, archivoSalida);
	fwrite(&bfOffBits, sizeof(int), 1, archivoSalida);
	fwrite(&biSize, sizeof(int), 1, archivoSalida);
	fwrite(&biWidth, sizeof(int), 1, archivoSalida);
	fwrite(&biHeight, sizeof(int), 1, archivoSalida);
	fwrite(&biPlanes, sizeof(short), 1, archivoSalida);
	fwrite(&biBitCount, sizeof(short), 1, archivoSalida);
	fwrite(&biCompression, sizeof(int), 1, archivoSalida);
	fwrite(&biSizeImage, sizeof(int), 1, archivoSalida);
	fwrite(&biXPelsPerMeter, sizeof(int), 1, archivoSalida);
	fwrite(&biYPelsPerMeter, sizeof(int), 1, archivoSalida);
	fwrite(&biClrUsed, sizeof(int), 1, archivoSalida);
	fwrite(&biClrImportant, sizeof(int), 1, archivoSalida);

	// Se escriben en el archivo los datos RGB de la imagen.
	for (y = 0; y < imagen->alto; y++) {
		for (x = 0; x < imagen->ancho; x++) {
			int pos = y * imagen->ancho + x;
			fwrite(&imagen->informacion[pos], sizeof(unsigned char), 1, archivoSalida);
		}
		fwrite(&relleno, sizeof(unsigned char), residuo, archivoSalida);
	}
	fclose(archivoSalida);
}