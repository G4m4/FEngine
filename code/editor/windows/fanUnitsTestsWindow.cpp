#include "editor/windows/fanUnitsTestsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/unit_tests/fanUnitTestCurrency.hpp"
#include "render/unit_tests/fanUnitTestMeshManager.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    UnitTestsWindow::UnitTestsWindow() : EditorWindow( "unit tests", ImGui::IconType::NONE ) {}

    //========================================================================================================
    //========================================================================================================
    std::vector<UnitTestsWindow::TestArgument> UnitTestsWindow::GetTests() {
        return {
                { "Currency", &UnitTestCurrency::RunTests,        mCurrencyResult },
                { "Mesh manager", &UnitTestMeshManager::RunTests, mMeshManagerResult },
        };
    }

    //========================================================================================================
    // draw all singletons of the ecs world
    //========================================================================================================
    void UnitTestsWindow::OnGui( EcsWorld& /*_world*/ )
    {
        SCOPED_PROFILE( unit_tests_window );

       const std::vector<TestArgument> tests = GetTests();

        if( ImGui::Button("Test all"))
        {
            for( const TestArgument& testArgument : tests ){ RunTest( testArgument ); }
        }
        ImGui::SameLine();
        if( ImGui::Button("Clear all"))
        {
            for( const TestArgument& testArgument : tests ){ ClearTest( testArgument ); }
        }
        ImGui::SameLine();
        ImGui::Checkbox("enable break", &UnitTestsUtils::sBreakWhenUnitTestFails );
        ImGui::Spacing();
        for( const TestArgument& testArgument : tests ){ DrawUnitTest( testArgument ); }
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::RunTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay =  ( *_testArgument.mRunMethod )();
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::ClearTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay = {};
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawUnitTest( const TestArgument& _testArgument )
    {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3  );
        DrawStatusIcon( _testArgument.mTestDisplay.mTotalStatus );
        ImGui::SameLine();
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3  );
        if( ImGui::CollapsingHeader( _testArgument.mName ) )
        {
            ImGui::Indent();
            if( ImGui::Button( "Test" ) ){ RunTest( _testArgument ); }
            ImGui::SameLine();
            if( ImGui::Button( "Clear" ) ){ ClearTest( _testArgument ); }

            ImGui::Columns( 2 );
            for( const UnitTestResult::TestResult& testResult : _testArgument.mTestDisplay.mTestDisplays )
            {
                DrawTest( testResult );
            }
            ImGui::Columns( 1 );
            ImGui::Unindent();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawTest( const UnitTestResult::TestResult& _testResult )
    {
        ImGui::Text( "%s", _testResult.mName.c_str() );
        ImGui::NextColumn();
        DrawStatusIcon( _testResult.mStatus );
        ImGui::NextColumn();
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawStatusIcon( const UnitTestResult::Status _status )
    {
        const ImVec2 iconSize = { 16, 16 };
        switch( _status )
        {
            case UnitTestResult::Status::Unknown :
                ImGui::Icon(  ImGui::IconType::CHECK_NEUTRAL16, iconSize, ImVec4( 1, 1, 1, 1 ) );
                break;
            case UnitTestResult::Status::Failed :
                ImGui::Icon(  ImGui::IconType::CHECK_FAILED16, iconSize, ImVec4( 1, 0, 0, 1 ) );
                break;
            case UnitTestResult::Status::Success :
                ImGui::Icon(  ImGui::IconType::CHECK_SUCCESS16, iconSize, ImVec4( 0, 1, 0, 1 ) );
                break;
            default:
                assert( false );
                break;
        }
    }
}