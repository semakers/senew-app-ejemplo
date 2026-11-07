/* ejemplo.c — la app de ejemplo del SDK de apps cargables (un cuentavueltas). OK suma una vuelta y apunta la hora; la tecla izquierda enciende o apaga la luz 1; C pone a
 * cero. Enseña lo básico: una pantalla con estado, un temporizador que muere con ella, teclas, la hora y un poco de hardware
 * por la API (nada del sistema por dentro). No viene de fábrica: se compila y se sube con SENUS. */
#include "w5.h"
#include "w5_pantalla.h"   /* (1.7) la letra nueva */

W5_APP(.nombre = "Cuentavueltas", .version = 2, .api_menor = W5_PANT_MENOR, .caps = W5_CAP_GIRA, .icono_gen = W5_IC_RELOJ);   /* la luz es opcional: sin luces, ap_luz da W5_NO_DISPONIBLE */

static int vueltas;
static W5Fecha ultima;                                                 /* su estado: estáticos de la app (cada carga, en frío) */
static unsigned inicio;

static void cada_segundo(W5Ctx c) { ui_sucio(c); }                    /* reconstruir para refrescar el tiempo */
static void iniciar(W5Ctx c, void *dato)
{
    (void)dato;
    inicio = ap_ms();
    ui_cada(c, 1000, cada_segundo);                                    /* el temporizador muere solo al cerrar la pantalla */
}
static int tecla(W5Ctx c, int k, void *dato)
{
    (void)dato;
    if (k == W5K_OK) { vueltas++; ap_fecha(&ultima); }                /* la hora local (el RTC lo lee el sistema) */
    else if (k == W5K_SEL_IZQ) ap_luz(1, ap_luz_estado(1) != 1);       /* (1.2) hardware por la API: la luz 1 */
    else if (k == W5K_C) vueltas = 0;
    else return ui_teclas_defecto(c, k);                               /* sel. der. = atrás; el resto, por defecto */
    ui_sucio(ui_mi_pantalla(c));
    return 1;                                                          /* tecla consumida */
}
static W5W construir(W5Ctx c, void *dato)
{
    unsigned seg = (ap_ms() - inicio) / 1000, tenue = ui_color(W5_COL_TENUE);
    (void)c; (void)dato;
    return Teclas(tecla, ui_andamio("Cuentavueltas",
        Columna(A_PRINCIPAL(W5P_CENTRO), A_CRUZADO(W5P_CENTRO), A_ESPACIO(8), HIJOS(
            Texto(ui_fmt("%d", vueltas), A_ESCALA(4), A_COLOR(ui_color(W5_COL_ACENTO))),
            Texto(vueltas ? ui_fmt("última: %02d:%02d:%02d", ultima.hora, ultima.min, ultima.seg) : "OK suma una vuelta"),
            Texto(ui_fmt("abierta hace %d:%02d", seg / 60, seg % 60), A_COLOR(tenue)),
            Texto(ap_luz_estado(1) == 1 ? "luz 1 encendida" : "luz 1 apagada", A_COLOR(tenue)))),
        "Luz", "Atrás"));
}
W5Pal app_suceso(int suceso, W5Pal a, W5Pal b)
{
    (void)a; (void)b;
    if (suceso == W5_ABRIR) return (W5Pal)ui_pantalla(construir, iniciar, NULL, NULL);
    return 0;
}
