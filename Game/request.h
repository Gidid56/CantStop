#pragma once

template<class Type>
class View
{
protected:
	void observe(Type const& view) { view_ = &view; }
	void reset() { view_ = nullptr; }
	auto view() const { return view_; }
	explicit operator bool() const { return view_; }

private:
	Type const* view_{ nullptr };
};

template<class Type, class... Views>
class Request : public View<Views>...
{
public:
	void reset()
	{
		(View<Views>::reset(), ...);
		promise_ = {};
		requesting_ = false;
	}

	auto request(Views const&... views)
	{
		requesting_ = false;
		(View<Views>::observe(views), ...);
		promise_ = {};
		requesting_ = true;

		return promise_.get_future();
	}

	void try_send(Type const& value)
	{
		if (isObserving()
			and requesting_.exchange(false))
		{
			(View<Views>::reset(), ...);
			promise_.set_value(value);
		}
	}

	template<class Type>
	auto view() const { return View<Type>::view(); }

	template<class Type>
	auto isObserving() const { return View<Type>::operator bool(); }

	auto isObserving() const { return (View<Views>::operator bool() and ...); }

	bool isRequesting() const { return requesting_; }

private:
	std::promise<Type> promise_;
	std::atomic<bool> requesting_{ false };
};
