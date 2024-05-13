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

return [
    'swow-extension' => [
        'version' => '1.5.2',
    ],
    'swow-library' => [
        /* No changes were made to the library during the upgrade
         * from extension version 1.5.1 to 1.5.2. */
        'version' => '1.5.1',
        /* library of v1.5.1 truly only required ^v1.5.1 extension,
         * because just some fixes were made to the extension. */
        'required_extension_version' => '^1.5.1',
    ],
];
