<?php
use PHPUnit\Framework\TestCase;

final class ApacheConfigurationTest extends TestCase {
    const CANONICAL_PARENT_DOMAIN_NAME = 'mediacommons.org';
    const CANONICAL_PROTOCOL = 'http';

    const INSTANCES = [
        'dev' => [
            'basename' => 'dev',
        ],
        'stage' => [
            'basename' => 'stage',
        ],
        'prod' => [
            'basename' => '',
        ]
    ];
    const PARENT_DOMAINS = [ 'mediacommons.org', 'media-commons.org' ];
    const PATHS = [ '', 'alt-ac', 'fieldguide', 'imr', 'intransition', 'mediacommons', 'tne' ];
    const PREFIXES = [ 'www', '' ];

    /**
     * @dataProvider generateTestUrls
     */
    public function testRedirect( $testUrl, $expectedEndUrl ) {
        list( $gotEndUrl ) = $this->checkRedirect( $testUrl, $expectedEndUrl );

        $this->assertEquals( $expectedEndUrl, $gotEndUrl );
    }

    public function generateTestUrls() {
        $testUrls = [];

        foreach ( self::INSTANCES as $instance ) {
            $basename = $instance[ 'basename' ];

            foreach(self::PARENT_DOMAINS as $parentDomain ) {

                foreach( self::PREFIXES as $prefix ) {
                    $subdomain = $basename;

                    if ( $prefix ) {
                        if ( $subdomain ) {
                            $subdomain = $prefix . '-' . $subdomain;
                        } else {
                            $subdomain = $prefix;
                        }
                    } else {
                        // Do nothing
                    }

                    foreach (self::PATHS as $path ) {

                        if ( $subdomain ) {
                            $canonicalFullyQualifiedDomainName = $basename . '.' . self::CANONICAL_PARENT_DOMAIN_NAME;
                            $fullyQualifiedDomainName = "${subdomain}.${parentDomain}";
                        } else {
                            $canonicalFullyQualifiedDomainName = self::CANONICAL_PARENT_DOMAIN_NAME;
                            $fullyQualifiedDomainName = "${parentDomain}";
                        }

                        if ( $path ) {
                            $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/${path}/";

                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/${path}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/${path}/", $canonicalUrl ] );
                        } else {
                            $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/";

                            array_push( $testUrls, [ "${fullyQualifiedDomainName}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/", $canonicalUrl ] );
                        }

                    }

                }

            }
        }

        return $testUrls;
    }

    public function checkRedirect( $url ) {
        $ch = curl_init();

        curl_setopt( $ch, CURLOPT_URL, $url );
        curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, TRUE );
        curl_setopt( $ch, CURLOPT_NOBODY, TRUE );

        curl_exec( $ch );

        $effectiveUrl = curl_getinfo( $ch, CURLINFO_EFFECTIVE_URL );

        // Sometimes the URL starts with "HTTP:".  Normalize to lowercase.
        $effectiveUrl = preg_replace( '/^HTTP/', 'http', $effectiveUrl );
        // We don't use https yet, but might in the future.
        $effectiveUrl = preg_replace( '/^HTTPS/', 'https', $effectiveUrl );

        return $effectiveUrl;
    }

}
