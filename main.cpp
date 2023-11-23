
#include <iostream>
#include <stdexcept>
#include <memory>

class ContBancar {
public:
    ContBancar(const std::string& detinator, double soldInitial);
    virtual ~ContBancar();

    virtual void afisareSold() const;
    virtual void retragere(double suma);

    static int numarConturiActive();

protected:
    std::string detinator_;
    mutable double sold_;

private:
    //atribut static
    static int numarConturiActive_;
};

class ExceptieRetragere : public std::exception {
public:
    const char* what() const noexcept override;
};

class ExceptieOverdraft : public std::exception {
public:
    const char* what() const noexcept override;
};

class ContCurent : public ContBancar {
public:
    ContCurent(const std::string& detinator, double soldInitial, double overdraft);
    ~ContCurent() override;

    void afisareSold() const override;
    void retragere(double suma) override;

private:
    double overdraft_;
};

class ContEconomii : public ContBancar {
public:
    ContEconomii(const std::string& detinator, double soldInitial, double dobanda);
    ~ContEconomii() override;

    void afisareSold() const override;
    void actualizareDobanda() const;

private:
    double dobanda_;
};

class ContCredit : public ContBancar {
public:
    ContCredit(const std::string& detinator, double soldInitial, double dobanda, double limitaCredit);
    ~ContCredit() override;

    void afisareSold() const override;
    void retragere(double suma) override;

private:
    double dobanda_;
    double limitaCredit_;
};

void operatiuneBancara(const ContBancar& cont);

int ContBancar::numarConturiActive_ = 0;

ContBancar::ContBancar(const std::string& detinator, double soldInitial)
    : detinator_(detinator), sold_(soldInitial) {
    std::cout << "Constructor ContBancar pentru " << detinator_ << "\n";
    ++numarConturiActive_;
}

ContBancar::~ContBancar() {
    std::cout << "Destructor ContBancar pentru " << detinator_ << "\n";
    --numarConturiActive_;
}

void ContBancar::afisareSold() const {
    std::cout << "Soldul pentru " << detinator_ << " este " << sold_ << " RON\n";
}

void ContBancar::retragere(double suma) {
    if (suma > 0 && suma <= sold_) {
        sold_ -= suma;
        std::cout << "Retragere de " << suma << " RON pentru " << detinator_ << "\n";
    } else {
        throw ExceptieRetragere();
    }
}

int ContBancar::numarConturiActive() {
    return numarConturiActive_;
}

const char* ExceptieRetragere::what() const noexcept {
    return "Retragere ilegala!";
}

const char* ExceptieOverdraft::what() const noexcept {
    return "Limita de overdraft depasita!";
}

ContCurent::ContCurent(const std::string& detinator, double soldInitial, double overdraft)
    : ContBancar(detinator, soldInitial), overdraft_(overdraft) {
    std::cout << "Constructor ContCurent pentru " << detinator_ << "\n";
}

ContCurent::~ContCurent() {
    std::cout << "Destructor ContCurent pentru " << detinator_ << "\n";
}

void ContCurent::afisareSold() const {
    std::cout << "Soldul curent pentru " << detinator_ << " este " << sold_ << " RON\n";
    std::cout << "Limita de overdraft: " << overdraft_ << " RON\n";
}

void ContCurent::retragere(double suma) {
    if (suma > 0 && (suma <= sold_ + overdraft_)) {
        sold_ -= suma;
        std::cout << "Retragere de " << suma << " RON pentru " << detinator_ << " (cont curent)\n";
    } else {
        throw ExceptieOverdraft();
    }
}

ContEconomii::ContEconomii(const std::string& detinator, double soldInitial, double dobanda)
    : ContBancar(detinator, soldInitial), dobanda_(dobanda) {
    std::cout << "Constructor ContEconomii pentru " << detinator_ << "\n";
}

ContEconomii::~ContEconomii() {
    std::cout << "Destructor ContEconomii pentru " << detinator_ << "\n";
}

void ContEconomii::afisareSold() const {
    std::cout << "Soldul economiilor pentru " << detinator_ << " este " << sold_ << " RON\n";
    std::cout << "Dobanda: " << dobanda_ << "%\n";
}

void ContEconomii::actualizareDobanda() const {
    double dobandaCalculata = sold_ * (dobanda_ / 100);
    sold_ += dobandaCalculata;
    std::cout << "Dobanda a fost actualizata pentru " << detinator_ << ". Soldul curent este acum: " << sold_ << " RON\n";
}

ContCredit::ContCredit(const std::string& detinator, double soldInitial, double dobanda, double limitaCredit)
    : ContBancar(detinator, soldInitial), dobanda_(dobanda), limitaCredit_(limitaCredit) {
    std::cout << "Constructor ContCredit pentru " << detinator_ << "\n";
}

ContCredit::~ContCredit() {
    std::cout << "Destructor ContCredit pentru " << detinator_ << "\n";
}

void ContCredit::afisareSold() const {
    std::cout << "Soldul creditului pentru " << detinator_ << " este " << sold_ << " RON\n";
    std::cout << "Dobanda: " << dobanda_ << "%\n";
    std::cout << "Limita de credit: " << limitaCredit_ << " RON\n";
}

void ContCredit::retragere(double suma) {
    if (suma > 0 && suma <= sold_ + limitaCredit_) {
        sold_ -= suma;
        std::cout << "Retragere de " << suma << " RON pentru " << detinator_ << " (cont credit)\n";
    } else {
        throw ExceptieRetragere();
    }
}

void operatiuneBancara(const ContBancar& cont) {
    const ContEconomii* contEconomii = dynamic_cast<const ContEconomii*>(&cont);
    if (contEconomii) {
        contEconomii->actualizareDobanda();
    } else {
        cont.afisareSold();
    }
}

int main() {
    try {
        ContCurent contCurent("Popescu Ioan ", 1000.0, 500.0);
        operatiuneBancara(contCurent);

        const ContEconomii contEconomii("Ionescu Mihai", 2000.0, 2.5);
        operatiuneBancara(contEconomii);

        std::cout << "Numarul total de conturi active: " << ContBancar::numarConturiActive() << std::endl;

        ContCredit contCredit("Alice", 1500.0, 2.0, 1000.0);
        operatiuneBancara(contCredit);

        contCurent.retragere(800.0);
    } catch (const std::exception& e) {
        std::cerr << "Exceptie: " << e.what() << std::endl;
    }

    std::cout << "Numarul total de conturi active: " << ContBancar::numarConturiActive() << std::endl;

    return 0;
}
