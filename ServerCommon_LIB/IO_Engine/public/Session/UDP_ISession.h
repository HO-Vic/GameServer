#pragma once

namespace sh::IO_Engine {
// UDP는 1:n 통신이 가능하기 때문에, Session당 소켓 x
// UDP_IAgent의 소켓을 사용하여 Send
class UDP_ISession {
};
}  // namespace sh::IO_Engine
