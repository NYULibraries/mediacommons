<?php
use PHPUnit\Framework\TestCase;

final class ApacheConfigurationTest extends TestCase {
    const CANONICAL_PARENT_DOMAIN_NAME = 'mediacommons.org';
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

    public function testTest() {
        $startUrl = "http://dev.media-commons.org/mediacommons/";
        $expectedEndUrl = "http://dev.mediacommons.org/mediacommons/";
        $expectedNumRedirects = 1;

        list( $effectiveUrl, $numRedirections ) = $this->checkRedirect( $startUrl );

        $this->assertEquals( $expectedEndUrl, $effectiveUrl );
        $this->assertEquals( $expectedNumRedirects, $numRedirections);
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
                            $canonicalUrl = $canonicalFullyQualifiedDomainName . "/${path}/";

                            $testUrls[ "${fullyQualifiedDomainName}/${path}" ] = $canonicalUrl;
                            $testUrls[ "${fullyQualifiedDomainName}/${path}/" ] = $canonicalUrl;
                        } else {
                            $canonicalUrl = $canonicalFullyQualifiedDomainName . '/';

                            $testUrls[ "${fullyQualifiedDomainName}" ] = $canonicalUrl;
                            $testUrls[ "${fullyQualifiedDomainName}/" ] = $canonicalUrl;
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
        $redirectCounts = curl_getinfo( $ch, CURLINFO_REDIRECT_COUNT );

        return [ $effectiveUrl, $redirectCounts ];
    }

}
