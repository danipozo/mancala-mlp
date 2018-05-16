template<class B, class A>
void AbstractPlayer<B, A>::update_state(const B& state)
{
    history.push_back(state);
}

template<class B, class A>
B AbstractPlayer<B, A>::current_state() const
{
    return history.back();
}
