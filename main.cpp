#include <iostream>
#include <tuple>
#include <string>

struct IAbstractBase {
    virtual void method() = 0;
};

struct IObject : IAbstractBase {
    virtual std::string id() const = 0;
};

struct ObjectImpl : IObject {
    std::string id_;

    explicit ObjectImpl(std::string id) : id_{ std::move(id) } {}

    std::string id() const override {
        return id_;
    }
};

struct IMeta : IAbstractBase {
    virtual int get_property() const = 0;
    virtual void set_property(int) = 0;
};

struct MetaImpl : IMeta {
    int property_{};

    explicit MetaImpl(int p) : property_{ p } {}

    int get_property() const override {
        return property_;
    }
    void set_property(int value) override {
        property_ = value;
    }
};

template<typename T>
struct AbstractBaseImpl : T {
    template <typename... Args>
    explicit AbstractBaseImpl(Args&&... args) : T(std::forward<Args>(args)...) {}

    void method() override {}
};

template <typename Tuple, std::size_t... ArgIdxs>
using tuple_indexes = std::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>;

template <typename Base>
struct Tupler : public Base {

    template<typename... Args, std::size_t... N>
    explicit Tupler(std::tuple<Args...>&& tuple, std::index_sequence<N...>) :
        Base{ std::get<N>(std::forward<std::tuple<Args...>>(tuple))... } {};
};

template<typename... T>
struct MetaObject : public T... {

    template<typename... Tuples>
    MetaObject(Tuples&&... tuples) : T{ std::forward<Tuples>(tuples),  tuple_indexes<Tuples>{} }... {}
};

template<typename... T, typename... Args>
auto MakeMetaObject(Args&&... args) -> AbstractBaseImpl<MetaObject<Tupler<T>...>>* {
    return new AbstractBaseImpl<MetaObject<Tupler<T>...>>{ std::forward<Args>(args)... };
}

int main(void)
{
    auto mo = MakeMetaObject<ObjectImpl, MetaImpl>(std::forward_as_tuple("id"), std::forward_as_tuple(5));
    auto o = static_cast<IObject *>(mo);
    auto m = static_cast<IMeta *>(mo);
}
