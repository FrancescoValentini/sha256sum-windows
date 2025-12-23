# sha256sum-windows
[🇺🇸 English version](README.md)
## 📖 Descrizione generale

**sha256sum-windows** è una utility da riga di comando per **Windows** progettata per calcolare l'hash **SHA-256** di uno o più file oppure di dati forniti tramite **stdin**.

Questo progetto **non ha alcuna relazione con le GNU coreutils**.
Il codice è una **implementazione sviluppata da zero**, che utilizza esclusivamente le **API crittografiche fornite dalla WinAPI**.

È pensato per funzionare **unicamente su Windows su piattaforma x86 e amd64**, **non sono previsti porting** verso altri sistemi operativi.

## ✨ Caratteristiche

- Calcolo dell'hash SHA-256 tramite **WinAPI Crypto**, senza dipendenze da librerie crittografiche esterne
- **Formato di output compatibile** con `sha256sum` su sistemi Unix/Linux
- Supporto sia per **file** sia per **stdin**
- Elaborazione di **uno o più file** in un'unica invocazione
- Utilizzabile in pipeline e script automatizzati

## 🚀 Utilizzo

Esempi di utilizzo comuni:

```bash
# Calcolo dell'hash
sha256sum-windows file.txt

sha256sum-windows file1.bin file2.bin

type file.txt | sha256sum-windows

sha256sum-windows file.txt > checksum.sha256

# Verifica degli hash
sha256sum-windows -c checksum.sha256
```


## 🧾 Formato di output

Il formato dell'output è **compatibile** con quello prodotto da `sha256sum` su sistemi Unix:

```
<hash_sha256>  <file>

336d4f9173bd4797e001b13e269f5fb2154715b5ee22f0260448aae6aab72149  ./file.pdf
```

Quando l'input proviene da **stdin**, il nome del file viene sostituito con `-`.

## 📦 Dipendenze esterne
- [https://github.com/p-ranav/argparse](https://github.com/p-ranav/argparse)

## ⚠️ Disclaimer
L'autore **non si assume alcuna responsabilità** per eventuali danni diretti o indiretti derivanti dall'uso del software, inclusi perdita di dati, usi illeciti, comportamenti inattesi o risultati errati.