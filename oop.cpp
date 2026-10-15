#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Library;

// CLASS BOOK
// Encapsulation: atribut hanya bisa diakses melalui getter.
class Book {
private:
    string id_;
    string title_;
    string author_;
    bool available_;

    friend class Library;

public:
    Book(const string& id, const string& title, const string& author)
        : id_(id), title_(title), author_(author), available_(true) {}

    const string& id() const { return id_; }
    const string& title() const { return title_; }
    const string& author() const { return author_; }
    bool isAvailable() const { return available_; }
};

// ABSTRACT CLASS MEMBER
// Abstraction: jenis anggota harus menentukan aturan pinjamnya.
class Member {
private:
    string id_;
    string name_;

    friend class Library;

public:
    Member(const string& id, const string& name) : id_(id), name_(name) {}
    virtual ~Member() {}

    const string& id() const { return id_; }
    const string& name() const { return name_; }

    virtual string memberType() const = 0;
    virtual int maxBooks() const = 0;
    virtual int loanDays() const = 0;
};

// INHERITANCE: STUDENT MEMBER
class StudentMember : public Member {
public:
    StudentMember(const string& id, const string& name)
        : Member(id, name) {}

    string memberType() const override { return "Mahasiswa"; }
    int maxBooks() const override { return 3; }
    int loanDays() const override { return 7; }
};

// INHERITANCE: LECTURER MEMBER
class LecturerMember : public Member {
public:
    LecturerMember(const string& id, const string& name)
        : Member(id, name) {}

    string memberType() const override { return "Dosen"; }
    int maxBooks() const override { return 5; }
    int loanDays() const override { return 14; }
};

struct Loan {
    string memberId;
    string bookId;
    bool active;
};

class Library {
private:
    vector<Book> books_;
    vector<unique_ptr<Member> > members_;
    vector<Loan> loans_;

    Book* findBook(const string& id) {
        for (size_t i = 0; i < books_.size(); ++i) {
            if (books_[i].id_ == id) return &books_[i];
        }
        return NULL;
    }

    const Book* findBook(const string& id) const {
        for (size_t i = 0; i < books_.size(); ++i) {
            if (books_[i].id_ == id) return &books_[i];
        }
        return NULL;
    }

    Member* findMember(const string& id) {
        for (size_t i = 0; i < members_.size(); ++i) {
            if (members_[i]->id_ == id) return members_[i].get();
        }
        return NULL;
    }

    const Member* findMember(const string& id) const {
        for (size_t i = 0; i < members_.size(); ++i) {
            if (members_[i]->id_ == id) return members_[i].get();
        }
        return NULL;
    }

    int activeLoansFor(const string& memberId) const {
        int count = 0;
        for (size_t i = 0; i < loans_.size(); ++i) {
            if (loans_[i].active && loans_[i].memberId == memberId) {
                ++count;
            }
        }
        return count;
    }

    static string lowercase(string value) {
        transform(value.begin(), value.end(), value.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        return value;
    }

public:
    // FEATURE 1: MANAJEMEN BUKU
    void addBook(const string& id, const string& title, const string& author) {
        if (id.empty() || title.empty() || author.empty()) {
            cout << "Gagal: ID, judul, dan penulis tidak boleh kosong.\n";
            return;
        }
        if (findBook(id) != NULL) {
            cout << "Gagal: ID buku sudah digunakan.\n";
            return;
        }
        books_.push_back(Book(id, title, author));
        cout << "Buku berhasil ditambahkan.\n";
    }

    void listBooks() const {
        if (books_.empty()) {
            cout << "Belum ada data buku.\n";
            return;
        }
        cout << "\n--- DAFTAR BUKU ---\n";
        for (size_t i = 0; i < books_.size(); ++i) {
            cout << "ID: " << books_[i].id_
                 << " | Judul: " << books_[i].title_
                 << " | Penulis: " << books_[i].author_
                 << " | Status: "
                 << (books_[i].available_ ? "Tersedia" : "Sedang dipinjam")
                 << '\n';
        }
    }

    void updateBook(const string& id, const string& title, const string& author) {
        Book* book = findBook(id);
        if (book == NULL) {
            cout << "Gagal: buku tidak ditemukan.\n";
            return;
        }
        if (title.empty() || author.empty()) {
            cout << "Gagal: judul dan penulis tidak boleh kosong.\n";
            return;
        }
        book->title_ = title;
        book->author_ = author;
        cout << "Data buku berhasil diperbarui.\n";
    }

    void deleteBook(const string& id) {
        for (vector<Book>::iterator it = books_.begin(); it != books_.end(); ++it) {
            if (it->id_ == id) {
                if (!it->available_) {
                    cout << "Gagal: buku sedang dipinjam dan tidak bisa dihapus.\n";
                    return;
                }
                books_.erase(it);
                cout << "Buku berhasil dihapus.\n";
                return;
            }
        }
        cout << "Gagal: buku tidak ditemukan.\n";
    }

    // FEATURE 2: MANAJEMEN ANGGOTA
    void addMember(char type, const string& id, const string& name) {
        if (id.empty() || name.empty()) {
            cout << "Gagal: ID dan nama anggota tidak boleh kosong.\n";
            return;
        }
        if (findMember(id) != NULL) {
            cout << "Gagal: ID anggota sudah digunakan.\n";
            return;
        }

        if (type == 'S' || type == 's') {
            members_.push_back(unique_ptr<Member>(new StudentMember(id, name)));
        } else if (type == 'D' || type == 'd') {
            members_.push_back(unique_ptr<Member>(new LecturerMember(id, name)));
        } else {
            cout << "Gagal: pilih S untuk mahasiswa atau D untuk dosen.\n";
            return;
        }
        cout << "Anggota berhasil didaftarkan.\n";
    }

    void listMembers() const {
        if (members_.empty()) {
            cout << "Belum ada data anggota.\n";
            return;
        }
        cout << "\n--- DAFTAR ANGGOTA ---\n";
        for (size_t i = 0; i < members_.size(); ++i) {
            const Member* member = members_[i].get();
            cout << "ID: " << member->id()
                 << " | Nama: " << member->name()
                 << " | Tipe: " << member->memberType()
                 << " | Pinjaman aktif: " << activeLoansFor(member->id())
                 << "/" << member->maxBooks()
                 << " | Durasi pinjam: " << member->loanDays() << " hari\n";
        }
    }

    void updateMember(const string& id, const string& newName) {
        Member* member = findMember(id);
        if (member == NULL) {
            cout << "Gagal: anggota tidak ditemukan.\n";
            return;
        }
        if (newName.empty()) {
            cout << "Gagal: nama tidak boleh kosong.\n";
            return;
        }
        member->name_ = newName;
        cout << "Data anggota berhasil diperbarui.\n";
    }

    void deleteMember(const string& id) {
        if (activeLoansFor(id) > 0) {
            cout << "Gagal: anggota masih memiliki pinjaman aktif.\n";
            return;
        }
        for (vector<unique_ptr<Member> >::iterator it = members_.begin();
             it != members_.end(); ++it) {
            if ((*it)->id() == id) {
                members_.erase(it);
                cout << "Anggota berhasil dihapus.\n";
                return;
            }
        }
        cout << "Gagal: anggota tidak ditemukan.\n";
    }

    // FEATURE 3: PENCARIAN BUKU berdasarkan ID, judul, atau penulis.
    void searchBooks(const string& query) const {
        if (query.empty()) {
            cout << "Kata pencarian tidak boleh kosong.\n";
            return;
        }

        const string q = lowercase(query);
        bool found = false;
        cout << "\n--- HASIL PENCARIAN ---\n";
        for (size_t i = 0; i < books_.size(); ++i) {
            const Book& book = books_[i];
            if (lowercase(book.id_).find(q) != string::npos ||
                lowercase(book.title_).find(q) != string::npos ||
                lowercase(book.author_).find(q) != string::npos) {
                cout << "ID: " << book.id_
                     << " | Judul: " << book.title_
                     << " | Penulis: " << book.author_
                     << " | Status: "
                     << (book.available_ ? "Tersedia" : "Sedang dipinjam")
                     << '\n';
                found = true;
            }
        }
        if (!found) cout << "Buku yang cocok tidak ditemukan.\n";
    }

    // FEATURE 4: PEMINJAMAN BUKU
    void borrowBook(const string& memberId, const string& bookId) {
        Member* member = findMember(memberId);
        Book* book = findBook(bookId);

        if (member == NULL) {
            cout << "Gagal: anggota tidak ditemukan.\n";
            return;
        }
        if (book == NULL) {
            cout << "Gagal: buku tidak ditemukan.\n";
            return;
        }
        if (!book->available_) {
            cout << "Gagal: buku sedang dipinjam.\n";
            return;
        }
        if (activeLoansFor(memberId) >= member->maxBooks()) {
            cout << "Gagal: anggota sudah mencapai batas peminjaman.\n";
            return;
        }

        book->available_ = false;
        Loan loan = { memberId, bookId, true };
        loans_.push_back(loan);
        cout << "Peminjaman berhasil. Batas durasi pinjam: "
             << member->loanDays() << " hari.\n";
    }

    // FEATURE 5: PENGEMBALIAN BUKU
    void returnBook(const string& memberId, const string& bookId) {
        Member* member = findMember(memberId);
        Book* book = findBook(bookId);

        if (member == NULL) {
            cout << "Gagal: anggota tidak ditemukan.\n";
            return;
        }
        if (book == NULL) {
            cout << "Gagal: buku tidak ditemukan.\n";
            return;
        }

        for (size_t i = 0; i < loans_.size(); ++i) {
            if (loans_[i].active && loans_[i].memberId == memberId &&
                loans_[i].bookId == bookId) {
                loans_[i].active = false;
                book->available_ = true;
                cout << "Pengembalian berhasil. Buku sekarang tersedia.\n";
                return;
            }
        }
        cout << "Gagal: tidak ditemukan peminjaman aktif untuk anggota dan buku tersebut.\n";
    }

    void listActiveLoans() const {
        bool found = false;
        cout << "\n--- PEMINJAMAN AKTIF ---\n";
        for (size_t i = 0; i < loans_.size(); ++i) {
            if (!loans_[i].active) continue;
            const Member* member = findMember(loans_[i].memberId);
            const Book* book = findBook(loans_[i].bookId);
            cout << "Anggota: "
                 << (member ? member->name() : loans_[i].memberId)
                 << " (" << loans_[i].memberId << ")"
                 << " | Buku: "
                 << (book ? book->title() : loans_[i].bookId)
                 << " (" << loans_[i].bookId << ")\n";
            found = true;
        }
        if (!found) cout << "Tidak ada peminjaman aktif.\n";
    }
};

string readInput(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

void showMenu() {
    cout << "\n========== LIBRARY MANAGEMENT SYSTEM ==========\n"
         << "1. Tambah buku\n"
         << "2. Tampilkan semua buku\n"
         << "3. Ubah data buku\n"
         << "4. Hapus buku\n"
         << "5. Daftarkan anggota\n"
         << "6. Tampilkan semua anggota\n"
         << "7. Ubah nama anggota\n"
         << "8. Hapus anggota\n"
         << "9. Cari buku\n"
         << "10. Pinjam buku\n"
         << "11. Kembalikan buku\n"
         << "12. Tampilkan peminjaman aktif\n"
         << "0. Keluar\n"
         << "===============================================\n";
}

int main() {
    Library library;

    while (true) {
        showMenu();
        string choice = readInput("Pilih menu: ");

        if (choice == "1") {
            string id = readInput("ID buku: ");
            string title = readInput("Judul buku: ");
            string author = readInput("Penulis: ");
            library.addBook(id, title, author);
        } else if (choice == "2") {
            library.listBooks();
        } else if (choice == "3") {
            string id = readInput("ID buku yang akan diubah: ");
            string title = readInput("Judul baru: ");
            string author = readInput("Penulis baru: ");
            library.updateBook(id, title, author);
        } else if (choice == "4") {
            string id = readInput("ID buku yang akan dihapus: ");
            library.deleteBook(id);
        } else if (choice == "5") {
            string type = readInput("Jenis anggota (S=Mahasiswa, D=Dosen): ");
            string id = readInput("ID anggota: ");
            string name = readInput("Nama anggota: ");
            library.addMember(type.empty() ? '\0' : type[0], id, name);
        } else if (choice == "6") {
            library.listMembers();
        } else if (choice == "7") {
            string id = readInput("ID anggota yang akan diubah: ");
            string name = readInput("Nama baru: ");
            library.updateMember(id, name);
        } else if (choice == "8") {
            string id = readInput("ID anggota yang akan dihapus: ");
            library.deleteMember(id);
        } else if (choice == "9") {
            string query = readInput("Masukkan ID, judul, atau penulis: ");
            library.searchBooks(query);
        } else if (choice == "10") {
            string memberId = readInput("ID anggota: ");
            string bookId = readInput("ID buku: ");
            library.borrowBook(memberId, bookId);
        } else if (choice == "11") {
            string memberId = readInput("ID anggota: ");
            string bookId = readInput("ID buku: ");
            library.returnBook(memberId, bookId);
        } else if (choice == "12") {
            library.listActiveLoans();
        } else if (choice == "0") {
            cout << "Program selesai.\n";
            break;
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }

    return 0;
}
