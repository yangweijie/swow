<?php
/**
 * This file is part of Swow
 *
 * @link    https://github.com/swow/swow
 * @contact twosee <twosee@php.net>
 *
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code
 */

declare(strict_types=1);

namespace Swow\Http;

use Swow\Buffer;
use Swow\Http\Server\Connection;
use Swow\Socket;
use Swow\Socket\Exception as SocketException;
use function is_string;

class Server extends Socket
{
    use ConfigTrait;

    protected array $connections = [];

    public function __construct()
    {
        parent::__construct(static::TYPE_TCP);
    }

    public function acceptConnection(?Connection $connection = null, int $timeout = null): Connection
    {
        if ($timeout === null) {
            $timeout = $this->getAcceptTimeout();
        }
        /* @var $connection Connection */
        $connection = parent::accept($connection ?? new Connection(), $timeout);
        $connection->setServer($this);
        $this->connections[$connection->getFd()] = $connection;

        return $connection;
    }

    /**
     * @param Connection[] $targets
     * @return SocketException[]
     */
    public function broadcastMessage(WebSocketFrame $frame, array $targets = null): array
    {
        /** @var Connection[] $targets */
        if ($targets === null) {
            $targets = $this->connections;
        }
        if ($frame->getPayloadLength() <= Buffer::PAGE_SIZE) {
            $frame = $frame->toString();
        }
        $exceptions = [];
        foreach ($targets as $target) {
            if ($target->getType() !== $target::TYPE_WEBSOCKET) {
                continue;
            }
            try {
                if (is_string($frame)) {
                    $target->sendString($frame);
                } else {
                    $target->sendWebSocketFrame($frame);
                }
            } catch (SocketException $exception) {
                /* record it and ignore */
                $exceptions[$target->getFd()] = $exception;
            }
        }

        return $exceptions;
    }

    public function offline(int $fd): void
    {
        unset($this->connections[$fd]);
    }

    public function closeSessions(): void
    {
        foreach ($this->connections as $connection) {
            $connection->close();
        }
        $this->connections = [];
    }

    public function close(): bool
    {
        $ret = parent::close();
        $this->closeSessions();

        return $ret;
    }

    public function __destruct()
    {
        $this->closeSessions();
    }
}