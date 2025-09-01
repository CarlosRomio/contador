#include <gtk/gtk.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <iostream>

// ----------------------
// Classe de controle
// ----------------------
class ContadorControl {
public:
    ContadorControl(GtkWidget* lbl)
        : contador(0), incremento(1), frequencia(1000),
        running(false), label(lbl) {
    }

    void start() {
        if (running) return;
        running = true;
        worker = std::thread(&ContadorControl::loop, this);
    }

    void stop() {
        running = false;
        if (worker.joinable())
            worker.join();
    }

    void reset() {
        contador = 0;
        atualizarUI();
    }

    void setIncremento(int inc) { incremento = inc; }
    void setFrequencia(int freq) { frequencia = freq; }

    int getContador() const { return contador; }

    ~ContadorControl() { stop(); }

private:
    std::atomic<int> contador;
    std::atomic<int> incremento;
    std::atomic<int> frequencia;
    std::atomic<bool> running;
    std::thread worker;
    GtkWidget* label;

    void loop() {
        while (running) {
            contador += incremento;
            atualizarUI();
            std::this_thread::sleep_for(std::chrono::milliseconds(frequencia));
        }
    }

    void atualizarUI() {
        g_idle_add([](gpointer data) -> gboolean {
            auto* self = static_cast<ContadorControl*>(data);
            std::string texto = "Contador: " + std::to_string(self->getContador());
            gtk_label_set_text(GTK_LABEL(self->label), texto.c_str());
            return G_SOURCE_REMOVE;
            }, this);
    }
};

// ----------------------
// Variáveis globais para UI
// ----------------------
GtkWidget* entryIncremento;
GtkWidget* entryFrequencia;
GtkWidget* labelContador;
ContadorControl* globalControl = nullptr;

// ----------------------
// Callbacks GTK
// ----------------------
void on_iniciar_parar(GtkWidget* widget, gpointer data) {
    static bool ativo = false;

    // antes de iniciar, lê os valores dos campos
    const char* inc_text = gtk_entry_get_text(GTK_ENTRY(entryIncremento));
    const char* freq_text = gtk_entry_get_text(GTK_ENTRY(entryFrequencia));

    int inc = atoi(inc_text);
    int freq = atoi(freq_text);

    if (inc > 0) globalControl->setIncremento(inc);
    if (freq > 0) globalControl->setFrequencia(freq);

    if (!ativo) {
        globalControl->start();
        gtk_button_set_label(GTK_BUTTON(widget), "Parar");
        ativo = true;
    }
    else {
        globalControl->stop();
        gtk_button_set_label(GTK_BUTTON(widget), "Iniciar");
        ativo = false;
    }
}

void on_reset(GtkWidget* widget, gpointer data) {
    globalControl->reset();
    gtk_label_set_text(GTK_LABEL(labelContador), "Contador: 0");
}

void destroy_cb(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

// ----------------------
// Main
// ----------------------
int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget* janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Contador com Thread");
    gtk_window_resize(GTK_WINDOW(janela), 500, 200);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // linha de inputs (incremento e frequência)
    GtkWidget* hboxInputs = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    entryIncremento = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryIncremento), "Incremento (ex: 1)");
    entryFrequencia = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entryFrequencia), "Frequência (ms)");

    gtk_box_pack_start(GTK_BOX(hboxInputs), entryIncremento, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hboxInputs), entryFrequencia, TRUE, TRUE, 5);

    // linha de botões
    GtkWidget* hboxButtons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* botaoStart = gtk_button_new_with_label("Iniciar");
    g_signal_connect(botaoStart, "clicked", G_CALLBACK(on_iniciar_parar), NULL);

    GtkWidget* botaoReset = gtk_button_new_with_label("Resetar");
    g_signal_connect(botaoReset, "clicked", G_CALLBACK(on_reset), NULL);

    gtk_box_pack_start(GTK_BOX(hboxButtons), botaoStart, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hboxButtons), botaoReset, TRUE, TRUE, 5);

    // label do contador
    labelContador = gtk_label_new("Contador: 0");

    // monta a interface
    gtk_box_pack_start(GTK_BOX(vbox), hboxInputs, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hboxButtons, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), labelContador, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(janela), vbox);

    g_signal_connect(janela, "destroy", G_CALLBACK(destroy_cb), NULL);

    gtk_widget_show_all(janela);

    // cria controle e passa o label
    globalControl = new ContadorControl(labelContador);

    gtk_main();

    globalControl->stop();
    delete globalControl;

    return 0;
}
