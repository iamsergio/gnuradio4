#ifndef GNURADIO_NULLSOURCES_HPP
#define GNURADIO_NULLSOURCES_HPP

#include <gnuradio-4.0/Block.hpp>
#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/DataSet.hpp>
#include <gnuradio-4.0/meta/reflection.hpp>

namespace gr::testing {

GR_REGISTER_BLOCK(gr::testing::NullSource, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct NullSource : Block<NullSource<T>> {
    using Description = Doc<R""(A source block that emits zeros or type-specified default value continuously.
Used mainly for testing and benchmarking where a consistent and predictable output is essential.
Ideal for scenarios that require a simple, low-overhead source of consistent values.)"">;

    gr::PortOut<T> out;

    GR_MAKE_REFLECTABLE(NullSource, out);

    [[nodiscard]] constexpr T processOne() const noexcept { return T{}; }
};

static_assert(gr::BlockLike<NullSource<float>>);

GR_REGISTER_BLOCK(gr::testing::ConstantSource, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct ConstantSource : Block<ConstantSource<T>> {
    using value_t     = std::conditional_t<std::is_same_v<T, std::string>, std::string, meta::fundamental_base_value_type_t<T>>; // use base-type for types not supported by settings
    using Description = Doc<R""(A source block that emits a constant default value for each output sample.
This block counts the number of samples emitted and optionally halts after reaching a specified maximum.
Commonly used for testing and simulations where consistent output and finite execution are required.)"">;

    gr::PortOut<T> out;

    Annotated<value_t, "default value", Visible, Doc<"default value for each sample">>            default_value{};
    Annotated<gr::Size_t, "max samples", Doc<"count>n_samples_max -> signal DONE (0: infinite)">> n_samples_max = 0U;
    Annotated<gr::Size_t, "count", Doc<"sample count (diagnostics only)">>                        count         = 0U;

    GR_MAKE_REFLECTABLE(ConstantSource, out, default_value, n_samples_max, count);

    void reset() { count = 0U; }

    [[nodiscard]] constexpr T processOne() noexcept {
        count++;
        if (n_samples_max > 0 && count >= n_samples_max) {
            this->requestStop();
        }
        return T(default_value.value);
    }
};

static_assert(gr::BlockLike<ConstantSource<float>>);

GR_REGISTER_BLOCK(gr::testing::SlowSource, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct SlowSource : Block<SlowSource<T>> {
    using value_t     = std::conditional_t<std::is_same_v<T, std::string>, std::string, meta::fundamental_base_value_type_t<T>>; // use base-type for types not supported by settings
    using Description = Doc<R""(A source block that emits a constant default value every n miliseconds)"">;

    gr::PortOut<T>                                                                              out;
    Annotated<value_t, "default value", Visible, Doc<"default value for each sample">>          default_value{};
    Annotated<gr::Size_t, "delay", Unit<"ms">, Doc<"how many milliseconds between each value">> delay = 100U;

    GR_MAKE_REFLECTABLE(SlowSource, out, default_value, delay);

    std::optional<std::chrono::time_point<std::chrono::system_clock>> lastEventAt;

    [[nodiscard]] gr::work::Status processBulk(OutputSpanLike auto& output) {
        if (!lastEventAt || std::chrono::system_clock::now() - *lastEventAt > std::chrono::milliseconds(delay)) {
            lastEventAt = std::chrono::system_clock::now();

            output[0] = T(default_value.value);
            output.publish(1UZ);
        } else {
            output.publish(0UZ);
        }

        return gr::work::Status::OK;
    }
};

GR_REGISTER_BLOCK(gr::testing::CountingSource, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double> ])

template<typename T>
struct CountingSource : Block<CountingSource<T>> {
    using Description = Doc<R""(A source block that emits an increasing sequence starting from a specified default value.
This block counts the number of samples emitted and optionally halts after reaching a specified maximum.
Commonly used for testing and simulations where consistent output and finite execution are required.)"">;

    gr::PortOut<T> out;

    Annotated<T, "default value", Visible, Doc<"default value for each sample">>                  default_value{};
    Annotated<gr::Size_t, "max samples", Doc<"count>n_samples_max -> signal DONE (0: infinite)">> n_samples_max = 0U;
    Annotated<gr::Size_t, "count", Doc<"sample count (diagnostics only)">>                        count         = 0U;

    GR_MAKE_REFLECTABLE(CountingSource, out, default_value, n_samples_max, count);

    void reset() { count = 0U; }

    [[nodiscard]] constexpr T processOne() noexcept {
        count++;
        if (n_samples_max > 0 && count >= n_samples_max) {
            this->requestStop();
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        return static_cast<T>(default_value.value) + static_cast<T>(count);
#pragma GCC diagnostic pop
    }
};

GR_REGISTER_BLOCK(gr::testing::Copy, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct Copy : Block<Copy<T>> {
    using Description = Doc<R""(A block that passes/copies input samples directly to its output without modification.
Commonly used used to isolate parts of a flowgraph, manage buffer sizes, or simply duplicate the signal path.)"">;

    gr::PortIn<T>  in;
    gr::PortOut<T> out;

    GR_MAKE_REFLECTABLE(Copy, in, out);

    template<gr::meta::t_or_simd<T> V>
    [[nodiscard]] constexpr auto processOne(V input) const noexcept {
        return input;
    }
};

GR_REGISTER_BLOCK(gr::testing::HeadBlock, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct HeadBlock : Block<HeadBlock<T>> {
    using Description = Doc<R""(Limits the number of output samples by copying the first N items from the input to the output and then signaling completion.
Commonly used to control data flow in systems where precise sample counts are critical, such as in file recording or when executing flow graphs without a GUI.)"">;

    gr::PortIn<T>  in;
    gr::PortOut<T> out;

    Annotated<gr::Size_t, "max samples", Doc<"count>n_samples_max -> signal DONE (0: infinite)">> n_samples_max = 0U;
    Annotated<gr::Size_t, "count", Doc<"sample count (diagnostics only)">>                        count         = 0U;

    GR_MAKE_REFLECTABLE(HeadBlock, in, out, n_samples_max, count);

    void reset() { count = 0U; }

    [[nodiscard]] constexpr auto processOne(T input) noexcept {
        count++;
        if (n_samples_max > 0 && count >= n_samples_max) {
            this->requestStop();
        }
        return input;
    }
};

GR_REGISTER_BLOCK(gr::testing::NullSink, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double>, gr::DataSet<uint16_t> ])

template<typename T>
struct NullSink : Block<NullSink<T>> {
    using Description = Doc<R""(A sink block that consumes and discards all input samples without producing any output.
Used primarily for absorbing data in a flow graph where output processing is unnecessary.
Commonly used for testing, performance benchmarking, and in scenarios where signal flow needs to be terminated without external effects.)"">;

    gr::PortIn<T> in;
    GR_MAKE_REFLECTABLE(NullSink, in);

    template<gr::meta::t_or_simd<T> V>
    void processOne(V) const noexcept { /* do nothing */ }
};

GR_REGISTER_BLOCK(gr::testing::CountingSink, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct CountingSink : Block<CountingSink<T>> {
    using Description = Doc<R""(A sink block that consumes and discards a fixed number of input samples, after which it signals the flow graph to halt.
This block is used to control execution in systems requiring precise input processing without data output, similar to how a 'HeadBlock' manages output samples.
Commonly used for testing scenarios and signal termination where output is unnecessary but precise input count control is needed.)"">;

    gr::PortIn<T> in;

    Annotated<gr::Size_t, "max samples", Doc<"count>n_samples_max -> signal DONE (0: infinite)">> n_samples_max = 0U;
    Annotated<gr::Size_t, "count", Doc<"sample count (diagnostics only)">>                        count         = 0U;

    GR_MAKE_REFLECTABLE(CountingSink, in, n_samples_max, count);

    void reset() { count = 0U; }

    void processOne(T) noexcept {
        count++;
        if (n_samples_max > 0 && count >= n_samples_max) {
            this->requestStop();
        }
    }
};

GR_REGISTER_BLOCK(gr::testing::SimCompute, [T], [ uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, gr::Packet<float>, gr::Packet<double>, gr::Tensor<float>, gr::Tensor<double>, gr::DataSet<float>, gr::DataSet<double> ])

template<typename T>
struct SimCompute : Block<SimCompute<T>> {
    using Description = Doc<R""(Block to simulate compute delay and model latency effects for a given complexity order

Delay depends on work size: delay_seconds = (N / reference_work_size)^complexity_order * (reference_work_size / target_throughput)

Default: linear O(N) delay that limits the block processing to 100 MS/s)"">;

    gr::PortIn<T>  in;
    gr::PortOut<T> out;

    Annotated<float, "target_throughput", Unit<"S/s">, Doc<"Target throughput in samples per second at reference_work_size.">>               target_throughput   = 100e6f;
    Annotated<gr::Size_t, "reference_work_size", Unit<"[N]">, Doc<"Reference work size N_r (samples) for which target_throughput applies.">> reference_work_size = 1024U;
    Annotated<float, "complexity_order", Doc<"compute complexity: 1.0 = linear, 2.0 = quadratic, 3.0 = cubic.">>                             complexity_order    = 1.0f;
    Annotated<bool, "busy_wait", Doc<"true: busy loop (CPU burn), False: sleep loop (lower power, less accurate).">>                         busy_wait           = true;

    GR_MAKE_REFLECTABLE(SimCompute, in, out, target_throughput, reference_work_size, complexity_order, busy_wait);

    [[nodiscard]] gr::work::Status processBulk(InputSpanLike auto& input, OutputSpanLike auto& output) {
        using Clock_t    = std::chrono::high_resolution_clock;
        const auto start = Clock_t::now();

        std::ranges::copy(input, output.begin());

        const std::chrono::duration<double> delay_seconds = compute_delay_seconds(input.size());
        const std::chrono::nanoseconds      delay_ns      = std::chrono::duration_cast<std::chrono::nanoseconds>(delay_seconds);
        while (Clock_t::now() - start < delay_ns) {
            if (!busy_wait) {
                std::this_thread::sleep_for(std::chrono::microseconds(10)); // avoid hard spin
            }
        }
        return gr::work::Status::OK;
    }

    [[nodiscard]] constexpr std::chrono::duration<double> compute_delay_seconds(std::size_t N) const noexcept {
        const auto N_r        = static_cast<double>(reference_work_size);
        const auto throughput = static_cast<double>(target_throughput);
        const auto k          = static_cast<double>(complexity_order);

        const double delay_s = std::pow(static_cast<double>(N) / N_r, k) * (N_r / throughput);

        return std::chrono::duration<double>(delay_s);
    }
};

} // namespace gr::testing

#endif // GNURADIO_NULLSOURCES_HPP
