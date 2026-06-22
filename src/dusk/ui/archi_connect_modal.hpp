#pragma once
#include "modal.hpp"

namespace dusk::ui
{


    class ArchiConnectModal : public Modal {
    public:
        enum class ConnectionStatus {
            None,
            Ready,
            Disconnecting,
            Connecting,
            Generating,
            Success,
            Error,
        };

        explicit ArchiConnectModal();
        void update() override;

    private:
        ConnectionStatus mDisplayedStatus = ConnectionStatus::None;
    };

    void BeginArchipelagoConnectionUI(bool forceChangeConnection = false);

}