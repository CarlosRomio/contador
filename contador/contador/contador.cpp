#include <gtk/gtk.h>
#include <stdio.h>
#include <iostream>

int contador = 0;

void click_botao(GtkWidget* widget, gpointer data) {
    contador++;
    std::cout << "Contador: " << contador << std::endl;
}

GtkWidget* criar_botao(void) {
    GtkWidget* botao = gtk_button_new();

    if (!botao) {
        printf("erro na criação de botão");
        return NULL;
    }

    gtk_button_set_label(GTK_BUTTON(botao), "nome botao");
    return botao;
}

void destroy_cb(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char* argv[]) {
    GtkWidget* janela = nullptr;

    gtk_init(&argc, &argv);

    janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (!janela) {
        g_print("erro ao criar janela\n");
        return 0;
    }

    gtk_window_set_title(GTK_WINDOW(janela), "Contador");
    gtk_window_resize(GTK_WINDOW(janela), 400, 500);

    GtkWidget* botao = criar_botao();

    // conecta o botão ao callback
    g_signal_connect(G_OBJECT(botao), "clicked", G_CALLBACK(click_botao), NULL);

    // conecta a janela ao callback de destruir
    g_signal_connect(G_OBJECT(janela), "destroy", G_CALLBACK(destroy_cb), NULL);

    gtk_container_add(GTK_CONTAINER(janela), botao);

    gtk_widget_show(botao);
    gtk_widget_show(janela);

    gtk_main();

    return 0;
}
