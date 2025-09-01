#include <gtk/gtk.h>
#include <iostream>
#include <string>

int contador = 0;
GtkWidget* label_contador = nullptr;
GtkWidget* label_tempo = nullptr;
gint64 ultimo_clique = 0; // em microssegundos

// callback do botão incrementar
void click_incrementar(GtkWidget* widget, gpointer data) {
    contador++;

    // atualiza label do contador
    std::string texto = "Contador: " + std::to_string(contador);
    gtk_label_set_text(GTK_LABEL(label_contador), texto.c_str());

    // calcula tempo desde o último clique
    gint64 agora = g_get_monotonic_time(); // microssegundos
    if (ultimo_clique > 0) {
        gint64 delta = agora - ultimo_clique;
        std::string tempo_str = "Tempo desde último clique: " + std::to_string(delta / 1000) + " ms";
        gtk_label_set_text(GTK_LABEL(label_tempo), tempo_str.c_str());
        std::cout << tempo_str << std::endl;
    }
    ultimo_clique = agora;

    std::cout << texto << std::endl;
}

// callback do botão resetar
void click_resetar(GtkWidget* widget, gpointer data) {
    contador = 0;
    gtk_label_set_text(GTK_LABEL(label_contador), "Contador: 0");
    gtk_label_set_text(GTK_LABEL(label_tempo), "Tempo desde último clique: -- ms");
    ultimo_clique = 0;
    std::cout << "Resetado" << std::endl;
}

// callback de fechar janela
void destroy_cb(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget* janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (!janela) {
        g_print("erro ao criar janela\n");
        return 0;
    }

    gtk_window_set_title(GTK_WINDOW(janela), "Contador com Reset e Timer");
    gtk_window_resize(GTK_WINDOW(janela), 500, 150);

    // container vertical (tudo empilhado)
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // container horizontal para os botões
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // botão incrementar
    GtkWidget* botao_incrementar = gtk_button_new_with_label("Incrementar");
    g_signal_connect(G_OBJECT(botao_incrementar), "clicked", G_CALLBACK(click_incrementar), NULL);

    // botão resetar
    GtkWidget* botao_resetar = gtk_button_new_with_label("Resetar");
    g_signal_connect(G_OBJECT(botao_resetar), "clicked", G_CALLBACK(click_resetar), NULL);

    // adiciona os botões lado a lado no hbox
    gtk_box_pack_start(GTK_BOX(hbox), botao_incrementar, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), botao_resetar, FALSE, FALSE, 5);

    // labels
    label_contador = gtk_label_new("Contador: 0");
    label_tempo = gtk_label_new("Tempo desde último clique: -- ms");

    // empilha tudo no vbox
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label_contador, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label_tempo, FALSE, FALSE, 5);

    // conecta janela
    g_signal_connect(G_OBJECT(janela), "destroy", G_CALLBACK(destroy_cb), NULL);

    // adiciona vbox na janela
    gtk_container_add(GTK_CONTAINER(janela), vbox);

    gtk_widget_show_all(janela);

    gtk_main();

    return 0;
}
