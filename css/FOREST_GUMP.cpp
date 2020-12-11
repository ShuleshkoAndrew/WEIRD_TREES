#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

template<class K, class V>
class AbstractTree
{
public:
    ~AbstractTree()=default;
virtual bool find(K k)=0;
virtual void insert(std::pair<K,V> p)=0;
virtual void remove(K k)=0;
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////////КРАСНО-ЧЕРНОЕ ДЕРЕВО/////////////////////////////////

enum class Color {
    Red,
    Black
};

template <class K, class V>
class Node {
public:
    K key;
    V value;
    Color color;
    std::unique_ptr<Node<K, V>> left;
    std::unique_ptr<Node<K, V>> right;
    Node<K, V>* parent;

    Node(const K& key, const V& val) : key {key}, value {val}, color {Color::Red}, parent {nullptr} {}
};

template <class K, class V>
class RBTree: public AbstractTree<K,V>
{
public:
    std::unique_ptr<Node<K, V>> root;

private:
    void LeftRotate(std::unique_ptr<Node<K, V>>&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        if (x->right) {
            x->right->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node<K, V>>(px);
            root->left->parent = root.get();
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node<K, V>>(px);
            xp->left->left->parent = xp->left.get();
        } else {
            auto px = x.release();
            xp->right = std::move(y);
            xp->right->left = std::unique_ptr<Node<K, V>>(px);
            xp->right->left->parent = xp->right.get();
        }
    }

    void RightRotate(std::unique_ptr<Node<K, V>>&& x) {
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        if (x->left) {
            x->left->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node<K, V>>(px);
            root->right->parent = root.get();
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node<K, V>>(px);
            xp->left->right->parent = xp->left.get();
        } else {
            auto px = x.release();
            xp->right = std::move(y);
            xp->right->right = std::unique_ptr<Node<K, V>>(px);
            xp->right->right->parent = xp->right.get();
        }
    }

public:
    /*
        find
        Принимает: ключ
        ищет узел по ключу
        Возвращает: указатель на найденный узел
    */
    bool find(K key) {
        return Search(root.get(), key);
    }

    /*
        insert
        Принимает: (ключ, значение)
        вставляет узел (ключ, значение)
        Возвращает: void
    */
    void insert(std::pair<K, V> p) {
    //void insert(const K& key, const V& val) {
        auto z = std::make_unique<Node<K, V>>(p.first, p.second);
        Insert(std::move(z));
    }

    /*
        remove
        Принимает: ключ
        проводит удаление узла по ключу
        Возвращает: 0  если успешно
                    -1 если такого узла нет
    */
    void remove(K key) {
        auto z = Search(root.get(), key);
        auto a = Delete(z);
    }

    /*
      print_node
      Принимает: ключ
      Выводит пару ключ-значение, берет их из find_node
    */
    void print_node (const K& key) {
      std::cout << find_node(key) -> key << " " << find_node(key) -> value << "\n";
    }

    int size(){
        return size(root.get());
    }

private:

    int size(Node<K,V> *p) {
        if (p == nullptr)
            return 0;
            return size(p->left.get()) + size(p->right.get()) + 1;
    }

    Node <K,V>* find_node(const K& key) {
      return Search(root.get(), key);
    }

    Node<K, V>* Search(Node<K, V>* x, K key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    void Insert(std::unique_ptr<Node<K, V>> z) {
        Node<K, V>* y = nullptr;
        Node<K, V>* x = root.get();
        while (x) {
            y = x;
            if (z->key < x->key) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        z->parent = y;
        if (!y) {
            root = std::move(z);
            InsertFixup(std::move(root));
        } else if (z->key < y->key) {
            y->left = std::move(z);
            InsertFixup(std::move(y->left));
        } else {
            y->right = std::move(z);
            InsertFixup(std::move(y->right));
        }
    }

    void InsertFixup(std::unique_ptr<Node<K, V>>&& z) {
        auto zp = z->parent;
        while (zp && zp->color == Color::Red) {
            auto zpp = zp->parent;
            if (zp == zpp->left.get()) {
                auto y = zpp->right.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    zp = zpp->parent;
                } else {
                    if (z == zp->right) {
                        LeftRotate(std::move(zpp->left));
                        zp = zpp->left.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    auto zppp = zpp->parent;
                    if (!zppp) {
                        RightRotate(std::move(root));
                    } else if (zpp == zppp->left.get()) {
                        RightRotate(std::move(zppp->left));
                    } else {
                        RightRotate(std::move(zppp->right));
                    }
                }
            } else {
                auto y = zpp->left.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    zp = zpp->parent;
                } else {
                    if (z == zp->left) {
                        RightRotate(std::move(zpp->right));
                        zp = zpp->right.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    auto zppp = zpp->parent;
                    if (!zppp) {
                        LeftRotate(std::move(root));
                    } else if (zpp == zppp->left.get()) {
                        LeftRotate(std::move(zppp->left));
                    } else {
                        LeftRotate(std::move(zppp->right));
                    }
                }
            }
        }
        root->color = Color::Black;
    }

    Node<K, V>* Transplant(Node<K, V>* u, std::unique_ptr<Node<K, V>>&& v) {
        if (v) {
            v->parent = u->parent;
        }
        Node<K, V>* w = nullptr;
        if (!u->parent) {
            w = root.release();
            root = std::move(v);
        } else if (u == u->parent->left.get()) {
            w = u->parent->left.release();
            u->parent->left = std::move(v);
        } else {
            w = u->parent->right.release();
            u->parent->right = std::move(v);
        }
        return w;
    }

    Node<K, V>* Minimum(Node<K, V>* x) {
        if (!x) {
            return x;
        }
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    int Delete(Node<K, V>* z) {
        if (!z) {
            return -1;
        }
        Color orig_color = z->color;
        Node<K, V>* x = nullptr;
        Node<K, V>* xp = nullptr;
        if (!z->left) {
            x = z->right.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->right));
            auto upz = std::unique_ptr<Node<K, V>>(pz);
        } else if (!z->right) {
            x = z->left.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->left));
            auto upz = std::unique_ptr<Node<K, V>>(pz);
        } else {
            auto y = Minimum(z->right.get());
            orig_color = y->color;
            x = y->right.get();
            xp = y;
            if (y->parent == z) {
                if (x) {
                    x->parent = y;
                }
                auto pz = Transplant(z, std::move(z->right));
                y->left = std::move(pz->left);
                y->left->parent = y;
                y->color = pz->color;
                auto upz = std::unique_ptr<Node<K, V>>(pz);
            } else {
                xp = y->parent;
                auto py = Transplant(y, std::move(y->right));
                py->right = std::move(z->right);
                py->right->parent = py;
                auto upy = std::unique_ptr<Node<K, V>>(py);
                auto pz = Transplant(z, std::move(upy));
                py->left = std::move(pz->left);
                py->left->parent = py;
                py->color = pz->color;
                auto upz = std::unique_ptr<Node<K, V>>(pz);
            }
        }
        if (orig_color == Color::Black) {
            DeleteFixup(x, xp);
        }

        return 0;
    }

    void DeleteFixup(Node<K, V>* x, Node<K, V>* xp) {
        while (x != root.get() && (!x || x->color == Color::Black)) {
            if (x == xp->left.get()) {
                Node<K, V>* w = xp->right.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    auto xpp = xp->parent;
                    if (!xpp) {
                        LeftRotate(std::move(root));
                    } else if (xp == xpp->left.get()) {
                        LeftRotate(std::move(xpp->left));
                    } else {
                        LeftRotate(std::move(xpp->right));
                    }
                    w = xp->right.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->right || w->right->color == Color::Black) {
                        w->left->color = Color::Black;
                        w->color = Color::Red;
                        auto wp = w->parent;
                        if (!wp) {
                            RightRotate(std::move(root));
                        } else if (w == wp->left.get()) {
                            RightRotate(std::move(wp->left));
                        } else {
                            RightRotate(std::move(wp->right));
                        }
                        w = xp->right.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->right->color = Color::Black;
                    auto xpp = xp->parent;
                    if (!xpp) {
                        LeftRotate(std::move(root));
                    } else if (xp == xpp->left.get()) {
                        LeftRotate(std::move(xpp->left));
                    } else {
                        LeftRotate(std::move(xpp->right));
                    }
                    x = root.get();
                } else {
                    x = root.get();
                }
            } else {
                Node<K, V>* w = xp->left.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    auto xpp = xp->parent;
                    if (!xpp) {
                        RightRotate(std::move(root));
                    } else if (xp == xpp->left.get()) {
                        RightRotate(std::move(xpp->left));
                    } else {
                        RightRotate(std::move(xpp->right));
                    }
                    w = xp->left.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->left || w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color = Color::Red;
                        auto wp = w->parent;
                        if (!wp) {
                            LeftRotate(std::move(root));
                        } else if (w == wp->left.get()) {
                            LeftRotate(std::move(wp->left));
                        } else {
                            LeftRotate(std::move(wp->right));
                        }
                        w = xp->left.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->left->color = Color::Black;
                    auto xpp = xp->parent;
                    if (!xpp) {
                        RightRotate(std::move(root));
                    } else if (xp == xpp->left.get()) {
                        RightRotate(std::move(xpp->left));
                    } else {
                        RightRotate(std::move(xpp->right));
                    }
                    x = root.get();
                } else {
                    x = root.get();
                }
            }
        }
        if (x) {
            x->color = Color::Black;
        }
    }

};

// Можно распечатать узел с его потомками в инфиксном порядке
template <class K, class V>
std::ostream& operator<<(std::ostream& os, Node<K, V>* node) {
    if (node) {
        os << node->left.get();
        os << ' ' << node->key << node->value;
        /*
        if (node->color == Color::Black) {
            os << "* "; // черное
        } else {
            os << "o "; // красное
        }
        */
        os << node->right.get();
    }
    return os;
}

// Можно распечатать дерево целиком в инфиксном порядке
template <class K, class V>
std::ostream& operator<<(std::ostream& os, const RBTree<K, V>& tree) {
    os << tree.root.get();
    return os;
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////АВЛ-ДЕРЕВО/////////////////////////////////////


template<class K, class V>
struct node {
    /*~node(){
        delete left; delete right;
    }*/
    K key;
    V val;

    int height;
    node<K,V> *left;
    node<K,V> *right;

    node<K,V>(K k,V v) {
        key = k;
        val = v;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
private:

    void CopyHelper(node<K,V> *node_copy, const node<K,V> *pNode) {
        if (pNode != nullptr) {
            node_copy->data = pNode->data;
            node_copy->height = pNode->height;

            if (pNode->left != nullptr) {
                auto node_copy_left = new node<K,V>;
                node_copy->left = node_copy_left;
            }

            if (pNode->right != nullptr) {
                auto node_copy_right = new node<K,V>;
                node_copy->right = node_copy_right;
            }

            CopyHelper(node_copy->left, pNode->left);
            CopyHelper(node_copy->right, pNode->right);
        }
    }

    node<K,V>(const node<K,V> &pNode) {
        CopyHelper(this, &pNode);
    }

    node<K,V> &operator=(const node<K,V> &pNode) {
        // self-assignment guard
        if (this == &pNode)
            return *this;

        // do the copy
        delete this->left;
        delete this->right;
        CopyHelper(this, &pNode);

        return *this;
    }
};

template<class K,class V>
class AVL : public AbstractTree<K,V>{
private:

    node<K,V> *root;

    int height(node<K,V> *p) {
        if (p == nullptr)
            return 0;
        return p->height;
    }

    int size(node<K,V> *p) {
        if (p == nullptr)
            return 0;
        return size(p->left) + size(p->right) + 1;
    }

    int bfactor(node<K,V> *p) {
        return height(p->right) - height(p->left);
    }

    void delete_all_tree(node<K,V> *node) {
            if (node->right != NULL)
                delete_all_tree(node->right);
            if (node->left != NULL)
                delete_all_tree(node->left);
            delete node;
    }

    void fixheight(node<K,V> *p) {
        int hl = height(p->left);
        int hr = height(p->right);
        if (hl > hr)
            p->height = hl + 1;
        else
            p->height = hr + 1;
    }

    node<K,V> *findmin(node<K,V> *p) {
        if (p->left == nullptr)
            return p;
        return findmin(p);
    }

    node<K,V> *balance(node<K,V> *p) {
        if (bfactor(p) == -2) {
            if (bfactor(p->left) > 0)
                p = doubleRightRotate(p);
            else
                p = singleLeftRotate(p);
            fixheight(p);
            return p;
        }
        if (bfactor(p) == 2) {
            if (bfactor(p->right) < 0)
                p = doubleLeftRotate(p);
            else
                p = singleLeftRotate(p);
            fixheight(p);
            return p;
        }
        fixheight(p);
        return p;
    }

    node<K,V> *removemin(node<K,V> *p) {
        if (p->left == nullptr)
            return p->right;
        p->left = removemin(p->left);
        return balance(p);
    }

    node<K,V> *insert(node<K,V> *p, K k,V v) {
        if (p == nullptr)
            return new node<K,V>(k,v);

        if (k < p->key) {
            p->left = insert(p->left,k, v);
            p = balance(p);

            return p;
        }

        if (k > p->key) {
            p->right = insert(p->right,k, v);
            p = balance(p);
            return p;
        }

        return p;
    }

    node<K,V> *singleRightRotate(node<K,V> *t) {
        if (t->left == nullptr)
            return t;
        node<K,V> *u = t->left;
        t->left = u->right;
        u->right = t;
        fixheight(t);
        fixheight(u);
        if (t == root)
            root = u;
        return u;
    }

    node<K,V> *singleLeftRotate(node<K,V> *t) {
        if (t->right == nullptr)
            return t;
        node<K,V> *u = t->right;
        t->right = u->left;
        u->left = t;
        fixheight(t);
        fixheight(u);
        if (t == root)
            root = u;
        return u;
    }

    node<K,V> *doubleLeftRotate(node<K,V> *t) {
        t->right = singleRightRotate(t->right);
        return singleLeftRotate(t);
    }

    node<K,V> *doubleRightRotate(node<K,V> *t) {
        t->left = singleLeftRotate(t->left);
        return singleRightRotate(t);
    }

    node<K,V> *remove(node<K,V> *p, K k) {
        if (p == nullptr)
            return nullptr;
        if (k < p->key) {
            p->left = remove(p->left,k);
            return balance(p);
        }
        if (k > p->key) {
            p->right = remove(p->right,k);
            return balance(p);
        }

        node<K,V> *l = p->left;
        node<K,V> *r = p->right;
        delete p;
        if (r == nullptr)
            return l;
        node<K,V> *newp = findmin(r);
        newp->right = removemin(r);
        newp->left = l;
        return balance(newp);
    }

    node<K,V> *find(node<K,V> *p, K k) {
        if (p == nullptr)
            return nullptr;
        if (p->key == k)
            return p;
        if (p->key < k)
            return find(p->right, k);
        return find(p->left, k);
    }
    node<V,K>* find_node(K k)
    {
        return (find(root,k));

    }
public:
    AVL() {
        root = nullptr;
    }

    ~AVL(){
        delete_all_tree(root);
    }

    void insert(std::pair<K,V> p) {
        root = insert(root, p.first,p.second);
    }

    void remove(K k) {
        root = remove(root, k);
    }

    bool find(K k) {
        return &(find(root,k)->key);
    }

    void print_node(K k)
    {
      if(find_node(k)!=nullptr){
    std::cout<<find_node(k)->key<<" "<<find_node(k)->val << "\n";
    }
    else std::cout<<"NO";
    }

    int size(){
        return size(root);
    }
};

////////////////////////////////////////////////////////////////////////////////

/*int main() {
RBTree<int,int> birch1;
   birch1.insert({566,52});
   birch1.insert({239,34});
   birch1.insert({1589,24});
   birch1.insert({299,37});
   birch1.insert({120,36});
   std::cout << (birch1.find(1589) ) << '\n';
   std::cout << (birch1.find(30) ) << '\n';
   std::cout << (birch1.find(239) ) << '\n';
   birch1.remove(2);
   birch1.remove(1589);
   birch1.insert({179,65});
   std::cout << birch1.find(179) << '\n';
   std::cout << birch1.find(1589) << '\n';
   std::cout << birch1.find(179) << '\n';
   std::cout << birch1.find(100) << '\n';

   std::cout << "##########################################\n";
   birch1.print_node(179);
   birch1.print_node(120);
   std::cout << birch1.size();
}*/

/*int main(){
  AVL<int,int> birch;
     birch.insert({566,52});
     birch.insert({239,34});
     birch.insert({1589,24});
     std::cout << (birch.find(1589) ) << '\n';
     std::cout << (birch.find(30) ) << '\n';
     std::cout << (birch.find(239) ) << '\n';
     birch.remove(2);
     birch.remove(1589);
     birch.insert({179,65});
     std::cout << birch.find(179) << '\n';
     std::cout << birch.find(1589) << '\n';

     std::cout << "##########################################\n";
     birch.print_node(179);
     std::cout << birch.size();
}*/
