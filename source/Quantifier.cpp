#include "Quantifier.h"

using namespace vis;

Quantifier::Quantifier() : Stage() {
  // ITK pipeline elements.
  m_converter = Converter::New();
  m_connector = BlobDetector::New();
  m_relabel = RelabelFilter::New();
  m_statistics = LabelStatistics::New();
  m_rng = VariateGenerator::New();

  m_relabel->SetInput(m_connector->GetOutput());
  m_statistics->SetLabelInput(m_relabel->GetOutput());
  m_converter->SetInput(m_relabel->GetOutput());

  m_connector->SetDistanceThreshold(4);
  m_relabel->SetMinimumObjectSize(4);
  m_statistics->UseHistogramsOn();

  BuildToolbox();
  BuildContent();
}

void Quantifier::BuildToolbox() {
  //TODO: needs UI stuff for MIP

  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  m_toolBox->setLayout(pageLayout);
}

void Quantifier::BuildContent() {
  // Create the main VTK view.
  m_view = new QVTKWidget();
  m_renderer = vtkRenderer::New();

  // Setup interaction and rendering.
  m_view->GetRenderWindow()->AddRenderer(m_renderer);
  m_interactor = m_view->GetInteractor();

  m_mapper = vtkSmartVolumeMapper::New();
  m_mapper->SetBlendModeToMaximumIntensity();
  m_mapper->SetRequestedRenderModeToRayCast();

  vtkColorTransferFunction* colorFunc = vtkColorTransferFunction::New();
  colorFunc->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 20000.0, 1.0, 0.5, 0.75);
  /*
  for (int i = 0; i < 55; i++) {
    colorFunc->AddRGBPoint((double)i * 5.0, 
      m_rng->GetUniformVariate(0.0, 255.0),
      m_rng->GetUniformVariate(0.0, 255.0),
      m_rng->GetUniformVariate(0.0, 255.0));
  }
  */

  vtkPiecewiseFunction* opacityFunc = vtkPiecewiseFunction::New();
  opacityFunc->AddSegment(0.0, 0.2, 255.0, 0.2);

  m_property = vtkVolumeProperty::New();
  m_property->SetColor(colorFunc);
  m_property->SetScalarOpacity(opacityFunc);
  m_property->SetInterpolationTypeToLinear();
  m_property->SetIndependentComponents(true);

  m_volume = vtkVolume::New();
  m_volume->SetMapper(m_mapper);
  m_volume->SetProperty(m_property);

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.2, 0.2, 0.2);
  m_renderer->SetBackground2(0.0, 0.0, 0.0);
  m_renderer->AddVolume(m_volume);
  m_renderer->ResetCamera();
  m_renderer->Render();
}

void Quantifier::SetImage(BaseImage::Pointer image) {
  m_connector->SetInput(image);
  m_statistics->SetInput(image);
  Quantify();
}

void Quantifier::Quantify() {
  m_statistics->Update();
  UpdateView();
}

void Quantifier::UpdateView() {
  m_converter->SetInput(m_relabel->GetOutput());
  m_converter->Update();
  m_converter->GetOutput()->PrintSelf(std::cout, vtkIndent());
  
  auto outImage = m_converter->GetOutput();
  for (int i = 0; i < 1000; i++) {

  }

  m_mapper->SetInputData(m_converter->GetOutput());
  m_mapper->Update();

  m_renderer->ResetCamera();
  m_renderer->Render();
}

QWidget* Quantifier::GetContent() {
  return m_view;
}

QWidget* Quantifier::GetToolbox() {
  return m_toolBox;
}
